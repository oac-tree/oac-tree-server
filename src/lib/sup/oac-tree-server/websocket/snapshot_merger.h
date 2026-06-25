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

#ifndef SUP_OAC_TREE_SERVER_SNAPSHOT_MERGER_H_
#define SUP_OAC_TREE_SERVER_SNAPSHOT_MERGER_H_

#include <sup/oac-tree/i_job_info_io.h>

#include <sup/dto/anyvalue.h>
#include <sup/dto/basic_scalar_types.h>

#include <set>
#include <string>
#include <vector>

namespace sup
{
namespace oac_tree_server
{
struct JobSnapshot;

/**
 * @brief IJobInfoIO decorator that lets live updates take precedence over a job snapshot.
 *
 * @details Every update forwarded through this decorator is recorded. When ApplySnapshot is called,
 * only entries that have not been delivered by a live update yet are forwarded to the wrapped
 * IJobInfoIO. This brings a client connecting to an already-running job up to date without the
 * snapshot overwriting more recent live updates, which may be delivered before (or interleaved with)
 * the snapshot.
 *
 * @note Not thread-safe: it is meant to be driven from a single thread (the client io thread that
 * dispatches job endpoint messages).
 */
class SnapshotMerger : public sup::oac_tree::IJobInfoIO
{
public:
  explicit SnapshotMerger(sup::oac_tree::IJobInfoIO& target);
  ~SnapshotMerger() override;

  // No copy or move
  SnapshotMerger(const SnapshotMerger&) = delete;
  SnapshotMerger(SnapshotMerger&&) = delete;
  SnapshotMerger& operator=(const SnapshotMerger&) = delete;
  SnapshotMerger& operator=(SnapshotMerger&&) = delete;

  /**
   * @brief Apply a job snapshot, forwarding only the entries not yet delivered by a live update.
   */
  void ApplySnapshot(const JobSnapshot& snapshot);

  void InitNumberOfInstructions(sup::dto::uint32 n_instr) override;
  void InstructionStateUpdated(sup::dto::uint32 instr_idx,
                               sup::oac_tree::InstructionState state) override;
  void BreakpointInstructionUpdated(sup::dto::uint32 instr_idx) override;
  void VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                       bool connected) override;
  void JobStateUpdated(sup::oac_tree::JobState state) override;
  void PutValue(const sup::dto::AnyValue& value, const std::string& description) override;
  bool GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                    const std::string& description) override;
  int GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                    const sup::dto::AnyValue& metadata) override;
  void Interrupt(sup::dto::uint64 id) override;
  void Message(const std::string& message) override;
  void Log(int severity, const std::string& message) override;
  void ProcedureTicked() override;

private:
  sup::oac_tree::IJobInfoIO& m_target;
  bool m_job_state_seen;
  bool m_breakpoint_seen;
  std::set<sup::dto::uint32> m_seen_instructions;
  std::set<sup::dto::uint32> m_seen_variables;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_SNAPSHOT_MERGER_H_
