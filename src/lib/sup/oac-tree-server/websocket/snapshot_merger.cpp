/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : oac-tree server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "snapshot_merger.h"

#include "ws_message.h"

#include <cstddef>

namespace
{
bool IsSeen(const std::vector<bool>& seen, sup::dto::uint32 idx)
{
  return (idx < seen.size()) && seen[idx];
}

void MarkSeen(std::vector<bool>& seen, sup::dto::uint32 idx)
{
  if (idx >= seen.size())
  {
    seen.resize(static_cast<std::size_t>(idx) + 1U, false);
  }
  seen[idx] = true;
}
}  // unnamed namespace

namespace sup
{
namespace oac_tree_server
{
using sup::oac_tree::InstructionState;

SnapshotMerger::SnapshotMerger(sup::oac_tree::IJobInfoIO& target)
  : m_target{target}
  , m_snapshot_applied{false}
  , m_job_state_seen{false}
  , m_breakpoint_seen{false}
  , m_seen_instructions{}
  , m_seen_variables{}
{}

SnapshotMerger::~SnapshotMerger() = default;

void SnapshotMerger::ApplySnapshot(const JobSnapshot& snapshot)
{
  // Only the first snapshot establishes the baseline; later ones are redundant because live updates
  // are authoritative from here on.
  if (m_snapshot_applied)
  {
    return;
  }
  if (!m_job_state_seen)
  {
    m_target.JobStateUpdated(snapshot.job_state);
  }
  if (!m_breakpoint_seen)
  {
    m_target.BreakpointInstructionUpdated(snapshot.current_breakpoint);
  }
  for (const auto& entry : snapshot.instructions)
  {
    if (!IsSeen(m_seen_instructions, entry.id))
    {
      m_target.InstructionStateUpdated(entry.id, entry.state);
    }
  }
  for (const auto& entry : snapshot.variables)
  {
    if (!IsSeen(m_seen_variables, entry.id))
    {
      m_target.VariableUpdated(entry.id, entry.value, entry.connected);
    }
  }
  // From now on updates delegate directly; release the per-id tracking flags.
  m_snapshot_applied = true;
  m_seen_instructions.clear();
  m_seen_instructions.shrink_to_fit();
  m_seen_variables.clear();
  m_seen_variables.shrink_to_fit();
}

void SnapshotMerger::InitNumberOfInstructions(sup::dto::uint32 n_instr)
{
  m_target.InitNumberOfInstructions(n_instr);
}

void SnapshotMerger::InstructionStateUpdated(sup::dto::uint32 instr_idx, InstructionState state)
{
  if (!m_snapshot_applied)
  {
    MarkSeen(m_seen_instructions, instr_idx);
  }
  m_target.InstructionStateUpdated(instr_idx, state);
}

void SnapshotMerger::BreakpointInstructionUpdated(sup::dto::uint32 instr_idx)
{
  if (!m_snapshot_applied)
  {
    m_breakpoint_seen = true;
  }
  m_target.BreakpointInstructionUpdated(instr_idx);
}

void SnapshotMerger::VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                                     bool connected)
{
  if (!m_snapshot_applied)
  {
    MarkSeen(m_seen_variables, var_idx);
  }
  m_target.VariableUpdated(var_idx, value, connected);
}

void SnapshotMerger::JobStateUpdated(sup::oac_tree::JobState state)
{
  if (!m_snapshot_applied)
  {
    m_job_state_seen = true;
  }
  m_target.JobStateUpdated(state);
}

void SnapshotMerger::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{
  m_target.PutValue(value, description);
}

bool SnapshotMerger::GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                                  const std::string& description)
{
  return m_target.GetUserValue(id, value, description);
}

int SnapshotMerger::GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                                  const sup::dto::AnyValue& metadata)
{
  return m_target.GetUserChoice(id, options, metadata);
}

void SnapshotMerger::Interrupt(sup::dto::uint64 id)
{
  m_target.Interrupt(id);
}

void SnapshotMerger::Message(const std::string& message)
{
  m_target.Message(message);
}

void SnapshotMerger::Log(int severity, const std::string& message)
{
  m_target.Log(severity, message);
}

void SnapshotMerger::ProcedureTicked()
{
  m_target.ProcedureTicked();
}

}  // namespace oac_tree_server

}  // namespace sup
