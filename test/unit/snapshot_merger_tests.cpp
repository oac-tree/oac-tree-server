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

#include <sup/oac-tree-server/websocket/snapshot_merger.h>

#include <sup/oac-tree-server/websocket/ws_message.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

using ::testing::Exactly;
using ::testing::Return;
using ::testing::_;

using namespace sup::oac_tree_server;

using sup::oac_tree::ExecutionStatus;
using sup::oac_tree::InstructionState;
using sup::oac_tree::JobState;

namespace
{
JobSnapshot MakeSnapshot()
{
  JobSnapshot snapshot{};
  snapshot.job_state = JobState::kRunning;
  snapshot.current_breakpoint = 5u;
  snapshot.instructions.push_back(
      InstructionStateEntry{0u, InstructionState{false, ExecutionStatus::NOT_STARTED}});
  snapshot.instructions.push_back(
      InstructionStateEntry{1u, InstructionState{true, ExecutionStatus::SUCCESS}});
  snapshot.variables.push_back(
      VariableStateEntry{0u, true, sup::dto::AnyValue{sup::dto::SignedInteger32Type, 1}});
  snapshot.variables.push_back(
      VariableStateEntry{1u, false, sup::dto::AnyValue{sup::dto::SignedInteger32Type, 2}});
  return snapshot;
}
}  // namespace

TEST(SnapshotMergerTest, AppliesFullSnapshotWhenNoLiveUpdatesSeen)
{
  ::testing::StrictMock<UnitTestHelper::MockJobInfoIO> target;
  SnapshotMerger merger{target};

  EXPECT_CALL(target, JobStateUpdated(JobState::kRunning)).Times(Exactly(1));
  EXPECT_CALL(target, BreakpointInstructionUpdated(5u)).Times(Exactly(1));
  EXPECT_CALL(target, InstructionStateUpdated(0u, _)).Times(Exactly(1));
  EXPECT_CALL(target, InstructionStateUpdated(1u, _)).Times(Exactly(1));
  EXPECT_CALL(target, VariableUpdated(0u, _, true)).Times(Exactly(1));
  EXPECT_CALL(target, VariableUpdated(1u, _, false)).Times(Exactly(1));

  merger.ApplySnapshot(MakeSnapshot());
}

TEST(SnapshotMergerTest, LiveUpdatesTakePrecedenceOverSnapshot)
{
  ::testing::StrictMock<UnitTestHelper::MockJobInfoIO> target;
  SnapshotMerger merger{target};

  // A live update for instruction 1, variable 0, the job state and the breakpoint arrives first.
  const InstructionState live_instr{true, ExecutionStatus::FAILURE};
  const sup::dto::AnyValue live_var{sup::dto::SignedInteger32Type, 99};
  EXPECT_CALL(target, JobStateUpdated(JobState::kPaused)).Times(Exactly(1));
  EXPECT_CALL(target, BreakpointInstructionUpdated(7u)).Times(Exactly(1));
  EXPECT_CALL(target, InstructionStateUpdated(1u, live_instr)).Times(Exactly(1));
  EXPECT_CALL(target, VariableUpdated(0u, live_var, true)).Times(Exactly(1));
  // The snapshot must then only deliver the entries not yet seen: instruction 0 and variable 1.
  EXPECT_CALL(target, InstructionStateUpdated(0u, _)).Times(Exactly(1));
  EXPECT_CALL(target, VariableUpdated(1u, _, false)).Times(Exactly(1));

  merger.JobStateUpdated(JobState::kPaused);
  merger.BreakpointInstructionUpdated(7u);
  merger.InstructionStateUpdated(1u, live_instr);
  merger.VariableUpdated(0u, live_var, true);

  merger.ApplySnapshot(MakeSnapshot());
}

TEST(SnapshotMergerTest, LiveUpdatesAfterSnapshotAreForwarded)
{
  ::testing::NiceMock<UnitTestHelper::MockJobInfoIO> target;
  SnapshotMerger merger{target};

  merger.ApplySnapshot(MakeSnapshot());

  // Updates arriving after the snapshot are always forwarded.
  const InstructionState later{true, ExecutionStatus::RUNNING};
  EXPECT_CALL(target, InstructionStateUpdated(1u, later)).Times(Exactly(1));
  EXPECT_CALL(target, JobStateUpdated(JobState::kSucceeded)).Times(Exactly(1));
  merger.InstructionStateUpdated(1u, later);
  merger.JobStateUpdated(JobState::kSucceeded);
}

TEST(SnapshotMergerTest, SecondSnapshotIsIgnored)
{
  ::testing::StrictMock<UnitTestHelper::MockJobInfoIO> target;
  SnapshotMerger merger{target};

  // The first snapshot establishes the baseline and forwards every entry once.
  EXPECT_CALL(target, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(target, BreakpointInstructionUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(target, InstructionStateUpdated(_, _)).Times(Exactly(2));
  EXPECT_CALL(target, VariableUpdated(_, _, _)).Times(Exactly(2));

  merger.ApplySnapshot(MakeSnapshot());
  // A second snapshot is redundant and must not forward anything (StrictMock catches extra calls).
  merger.ApplySnapshot(MakeSnapshot());
}

TEST(SnapshotMergerTest, NonCachedUpdatesAreForwarded)
{
  ::testing::StrictMock<UnitTestHelper::MockJobInfoIO> target;
  SnapshotMerger merger{target};

  const sup::dto::AnyValue value{sup::dto::BooleanType, true};
  EXPECT_CALL(target, PutValue(value, std::string{"desc"})).Times(Exactly(1));
  EXPECT_CALL(target, Message(std::string{"hi"})).Times(Exactly(1));
  EXPECT_CALL(target, Log(3, std::string{"log"})).Times(Exactly(1));

  merger.PutValue(value, "desc");
  merger.Message("hi");
  merger.Log(3, "log");
}

TEST(SnapshotMergerTest, InitInterruptTickAndUserInputForwarded)
{
  ::testing::StrictMock<UnitTestHelper::MockJobInfoIO> target;
  SnapshotMerger merger{target};

  EXPECT_CALL(target, InitNumberOfInstructions(7u)).Times(Exactly(1));
  EXPECT_CALL(target, ProcedureTicked()).Times(Exactly(1));
  EXPECT_CALL(target, Interrupt(9u)).Times(Exactly(1));
  EXPECT_CALL(target, GetUserValue(1u, _, std::string{"d"})).Times(Exactly(1)).WillOnce(Return(true));
  EXPECT_CALL(target, GetUserChoice(2u, _, _)).Times(Exactly(1)).WillOnce(Return(3));

  merger.InitNumberOfInstructions(7u);
  merger.ProcedureTicked();
  merger.Interrupt(9u);
  sup::dto::AnyValue value{sup::dto::SignedInteger32Type, 0};
  EXPECT_TRUE(merger.GetUserValue(1u, value, "d"));
  const std::vector<std::string> options{"a", "b"};
  EXPECT_EQ(merger.GetUserChoice(2u, options, sup::dto::AnyValue{}), 3);
}
