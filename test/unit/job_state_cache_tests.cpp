/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : Unit test code
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

#include <sup/oac-tree-server/websocket/job_state_cache.h>

#include <sup/oac-tree-server/websocket/ws_message_handler.h>

#include <sup/dto/anyvalue_helper.h>

#include <gtest/gtest.h>

#include <atomic>
#include <thread>

using namespace sup::oac_tree_server;

using sup::oac_tree::ExecutionStatus;
using sup::oac_tree::InstructionState;
using sup::oac_tree::JobState;

namespace
{
JobSnapshot ParseSnapshot(const std::string& msg)
{
  auto parsed = ParseJobSnapshotMessage(msg);
  EXPECT_TRUE(parsed.has_value());
  return parsed.value_or(JobSnapshot{});
}
}  // namespace

TEST(JobStateCacheTest, DefaultsBeforeAnyUpdate)
{
  JobStateCache cache{2u};
  cache.SetInstructionCount(3u);

  const auto snapshot = ParseSnapshot(cache.BuildSnapshotMessage());
  EXPECT_EQ(snapshot.job_state, JobState::kInitial);
  EXPECT_EQ(snapshot.current_breakpoint, sup::oac_tree::kInvalidInstructionIndex);

  ASSERT_EQ(snapshot.instructions.size(), 3u);
  for (sup::dto::uint32 i = 0u; i < 3u; ++i)
  {
    EXPECT_EQ(snapshot.instructions[i].id, i);
    EXPECT_FALSE(snapshot.instructions[i].state.m_breakpoint_set);
    EXPECT_EQ(snapshot.instructions[i].state.m_execution_status, ExecutionStatus::NOT_STARTED);
  }
  ASSERT_EQ(snapshot.variables.size(), 2u);
  for (sup::dto::uint32 i = 0u; i < 2u; ++i)
  {
    EXPECT_EQ(snapshot.variables[i].id, i);
    EXPECT_FALSE(snapshot.variables[i].connected);
    EXPECT_TRUE(sup::dto::IsEmptyValue(snapshot.variables[i].value));
  }
}

TEST(JobStateCacheTest, NoVariablesOrInstructions)
{
  JobStateCache cache{0u};

  const auto snapshot = ParseSnapshot(cache.BuildSnapshotMessage());
  EXPECT_TRUE(snapshot.instructions.empty());
  EXPECT_TRUE(snapshot.variables.empty());
}

TEST(JobStateCacheTest, ReflectsUpdates)
{
  JobStateCache cache{2u};
  cache.SetInstructionCount(3u);
  cache.SetJobState(JobState::kRunning);
  cache.SetCurrentBreakpoint(1u);
  cache.SetInstructionState(1u, InstructionState{true, ExecutionStatus::SUCCESS});
  const sup::dto::AnyValue var0{sup::dto::SignedInteger32Type, 42};
  cache.SetVariable(0u, var0, true);

  const auto snapshot = ParseSnapshot(cache.BuildSnapshotMessage());
  EXPECT_EQ(snapshot.job_state, JobState::kRunning);
  EXPECT_EQ(snapshot.current_breakpoint, 1u);

  ASSERT_EQ(snapshot.instructions.size(), 3u);
  EXPECT_TRUE(snapshot.instructions[1].state.m_breakpoint_set);
  EXPECT_EQ(snapshot.instructions[1].state.m_execution_status, ExecutionStatus::SUCCESS);
  // Untouched instructions keep their default state.
  EXPECT_EQ(snapshot.instructions[0].state.m_execution_status, ExecutionStatus::NOT_STARTED);
  EXPECT_EQ(snapshot.instructions[2].state.m_execution_status, ExecutionStatus::NOT_STARTED);

  ASSERT_EQ(snapshot.variables.size(), 2u);
  EXPECT_TRUE(snapshot.variables[0].connected);
  EXPECT_EQ(snapshot.variables[0].value, var0);
  // Untouched variable keeps its default (not connected, empty value).
  EXPECT_FALSE(snapshot.variables[1].connected);
  EXPECT_TRUE(sup::dto::IsEmptyValue(snapshot.variables[1].value));
}

TEST(JobStateCacheTest, LatestValueWins)
{
  JobStateCache cache{1u};
  cache.SetInstructionCount(1u);
  cache.SetInstructionState(0u, InstructionState{false, ExecutionStatus::RUNNING});
  cache.SetInstructionState(0u, InstructionState{true, ExecutionStatus::FAILURE});
  cache.SetVariable(0u, sup::dto::AnyValue{sup::dto::SignedInteger32Type, 1}, true);
  cache.SetVariable(0u, sup::dto::AnyValue{sup::dto::SignedInteger32Type, 2}, false);

  const auto snapshot = ParseSnapshot(cache.BuildSnapshotMessage());
  EXPECT_TRUE(snapshot.instructions[0].state.m_breakpoint_set);
  EXPECT_EQ(snapshot.instructions[0].state.m_execution_status, ExecutionStatus::FAILURE);
  EXPECT_FALSE(snapshot.variables[0].connected);
  const sup::dto::AnyValue expected{sup::dto::SignedInteger32Type, 2};
  EXPECT_EQ(snapshot.variables[0].value, expected);
}

TEST(JobStateCacheTest, OutOfRangeUpdatesIgnored)
{
  JobStateCache cache{1u};
  cache.SetInstructionCount(1u);
  // Out-of-range updates must be ignored without crashing or resizing.
  cache.SetInstructionState(99u, InstructionState{true, ExecutionStatus::SUCCESS});
  cache.SetVariable(99u, sup::dto::AnyValue{sup::dto::SignedInteger32Type, 7}, true);

  const auto snapshot = ParseSnapshot(cache.BuildSnapshotMessage());
  ASSERT_EQ(snapshot.instructions.size(), 1u);
  ASSERT_EQ(snapshot.variables.size(), 1u);
  EXPECT_EQ(snapshot.instructions[0].state.m_execution_status, ExecutionStatus::NOT_STARTED);
  EXPECT_FALSE(snapshot.variables[0].connected);
}

TEST(JobStateCacheTest, ConcurrentUpdatesAndSnapshotsDoNotRace)
{
  JobStateCache cache{4u};
  cache.SetInstructionCount(4u);
  std::atomic<bool> stop{false};
  std::thread writer([&cache, &stop]() {
    sup::dto::int32 counter = 0;
    while (!stop.load())
    {
      for (sup::dto::uint32 i = 0u; i < 4u; ++i)
      {
        cache.SetVariable(i, sup::dto::AnyValue{sup::dto::SignedInteger32Type, counter}, true);
        cache.SetInstructionState(i, InstructionState{false, ExecutionStatus::RUNNING});
      }
      ++counter;
    }
  });

  for (int i = 0; i < 1000; ++i)
  {
    EXPECT_FALSE(cache.BuildSnapshotMessage().empty());
  }

  stop.store(true);
  writer.join();
}
