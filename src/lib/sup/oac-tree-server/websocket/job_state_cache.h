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

#ifndef SUP_OAC_TREE_SERVER_JOB_STATE_CACHE_H_
#define SUP_OAC_TREE_SERVER_JOB_STATE_CACHE_H_

#include <sup/oac-tree/i_job_info_io.h>

#include <sup/dto/anyvalue.h>
#include <sup/dto/basic_scalar_types.h>

#include <mutex>
#include <string>
#include <vector>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief Thread-safe cache of the latest published state of a single job: job state, current
 * breakpoint, and the latest state of every instruction and variable. It produces a job snapshot
 * message so that a client connecting to an already-running job can be brought up to date.
 *
 * @details All members are protected by an internal mutex so that snapshot creation (typically on a
 * server io thread) and state updates (on the job execution thread) do not race. The mutex only
 * guards against data races; it does not impose any ordering between updates and snapshot reads.
 *
 * Instructions and variables that have not been updated yet keep their default state (instructions:
 * not started and no breakpoint; variables: not connected with an empty value), so the snapshot is
 * always complete and fixed-size.
 */
class JobStateCache
{
public:
  /**
   * @brief Construct a cache sized for the given number of variables. The number of instructions is
   * provided later via SetInstructionCount (it is not known at construction time).
   */
  explicit JobStateCache(sup::dto::uint32 n_vars);
  ~JobStateCache();

  // No copy or move
  JobStateCache(const JobStateCache&) = delete;
  JobStateCache(JobStateCache&&) = delete;
  JobStateCache& operator=(const JobStateCache&) = delete;
  JobStateCache& operator=(JobStateCache&&) = delete;

  /**
   * @brief Set the number of instructions, resetting every instruction to its default state.
   */
  void SetInstructionCount(sup::dto::uint32 n_instr);

  void SetJobState(sup::oac_tree::JobState state);

  /**
   * @brief Update the cached state of a single instruction. Out-of-range indices are ignored.
   */
  void SetInstructionState(sup::dto::uint32 instr_idx, sup::oac_tree::InstructionState state);

  void SetCurrentBreakpoint(sup::dto::uint32 instr_idx);

  /**
   * @brief Update the cached value and connected state of a single variable. Out-of-range indices
   * are ignored.
   */
  void SetVariable(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value, bool connected);

  /**
   * @brief Build a job snapshot message reflecting the currently cached state.
   */
  std::string BuildSnapshotMessage() const;

private:
  struct VariableState
  {
    bool connected{false};
    sup::dto::AnyValue value{};
  };

  mutable std::mutex m_mtx;
  sup::oac_tree::JobState m_job_state;
  sup::dto::uint32 m_current_breakpoint;
  std::vector<sup::oac_tree::InstructionState> m_instructions;
  std::vector<VariableState> m_variables;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_JOB_STATE_CACHE_H_
