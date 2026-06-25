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

#include "job_state_cache.h"

#include "ws_message.h"

#include <cstddef>

namespace sup
{
namespace oac_tree_server
{

JobStateCache::JobStateCache(sup::dto::uint32 n_vars)
  : m_mtx{}
  , m_job_state{sup::oac_tree::JobState::kInitial}
  , m_current_breakpoint{sup::oac_tree::kInvalidInstructionIndex}
  , m_instructions{}
  , m_variables(n_vars)
{}

JobStateCache::~JobStateCache() = default;

void JobStateCache::SetInstructionCount(sup::dto::uint32 n_instr)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  m_instructions.assign(n_instr, sup::oac_tree::InstructionState{});
}

void JobStateCache::SetJobState(sup::oac_tree::JobState state)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  m_job_state = state;
}

void JobStateCache::SetInstructionState(sup::dto::uint32 instr_idx,
                                        sup::oac_tree::InstructionState state)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  if (instr_idx < m_instructions.size())
  {
    m_instructions[instr_idx] = state;
  }
}

void JobStateCache::SetCurrentBreakpoint(sup::dto::uint32 instr_idx)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  m_current_breakpoint = instr_idx;
}

void JobStateCache::SetVariable(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                                bool connected)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  if (var_idx < m_variables.size())
  {
    m_variables[var_idx].value = value;
    m_variables[var_idx].connected = connected;
  }
}

std::string JobStateCache::BuildSnapshotMessage() const
{
  JobSnapshot snapshot{};
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    snapshot.job_state = m_job_state;
    snapshot.current_breakpoint = m_current_breakpoint;
    snapshot.instructions.reserve(m_instructions.size());
    for (std::size_t idx = 0; idx < m_instructions.size(); ++idx)
    {
      snapshot.instructions.push_back(
        InstructionStateEntry{static_cast<sup::dto::uint32>(idx), m_instructions[idx]});
    }
    snapshot.variables.reserve(m_variables.size());
    for (std::size_t idx = 0; idx < m_variables.size(); ++idx)
    {
      snapshot.variables.push_back(VariableStateEntry{
        static_cast<sup::dto::uint32>(idx), m_variables[idx].connected, m_variables[idx].value});
    }
  }
  // Serialize outside the lock to keep the critical section short.
  return CreateJobSnapshotMessage(snapshot);
}

}  // namespace oac_tree_server

}  // namespace sup
