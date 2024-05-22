/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVER
 *
 * Description   : Unit test code
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "unit_test_helper.h"

#include <sup/auto-server/server_job_interface.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/protocol/base64_variable_codec.h>

#include <sup/sequencer/instruction.h>
#include <sup/sequencer/job_controller.h>
#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

const std::string kTestPrefix = "ServerJobInterfaceTests";

class ServerJobInterfaceTest : public ::testing::Test
{
protected:
  ServerJobInterfaceTest() = default;
  virtual ~ServerJobInterfaceTest() = default;

  UnitTestHelper::TestServerInterface m_test_server_interface;
};

TEST_F(ServerJobInterfaceTest, Construction)
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_EQ(m_test_server_interface.GetSize(), 0);
  ServerJobInterface job_interface{kTestPrefix, *proc, m_test_server_interface};

  // Check presence of job state and 3 variable anyvalues:
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);
  EXPECT_TRUE(m_test_server_interface.HasAnyValue(GetJobStatePVName(kTestPrefix)));
  EXPECT_TRUE(m_test_server_interface.HasAnyValue(GetVariablePVName(kTestPrefix, 0)));
  EXPECT_TRUE(m_test_server_interface.HasAnyValue(GetVariablePVName(kTestPrefix, 1)));
  EXPECT_TRUE(m_test_server_interface.HasAnyValue(GetVariablePVName(kTestPrefix, 2)));

  // Check job state anyvalue is initial:
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetJobStatePVName(kTestPrefix)), kJobStateAnyValue);

  // Check variable anyvalues are empty:
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 0)),
            kVariableAnyValue);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 1)),
            kVariableAnyValue);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 2)),
            kVariableAnyValue);
}

TEST_F(ServerJobInterfaceTest, AfterSetup)
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_EQ(m_test_server_interface.GetSize(), 0);
  ServerJobInterface job_interface{kTestPrefix, *proc, m_test_server_interface};

  // Check variable anyvalues before setup:
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetJobStatePVName(kTestPrefix)), kJobStateAnyValue);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 0)),
            kVariableAnyValue);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 1)),
            kVariableAnyValue);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 2)),
            kVariableAnyValue);

  // Setup and check again (variables are no longer empty, but state is still initial)
  // The JobController is necessary as it binds the variable callbacks to the job interface
  sup::sequencer::JobController controller{*proc, job_interface};
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetJobStatePVName(kTestPrefix)), kJobStateAnyValue);
  EXPECT_NE(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 0)),
            kVariableAnyValue);
  EXPECT_NE(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 1)),
            kVariableAnyValue);
  EXPECT_NE(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 2)),
            kVariableAnyValue);
}

TEST_F(ServerJobInterfaceTest, InitializeInstructionTree)
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_EQ(m_test_server_interface.GetSize(), 0);
  ServerJobInterface job_interface{kTestPrefix, *proc, m_test_server_interface};

  // Check variable anyvalues before setup:
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);

  // Setup and check again
  sup::sequencer::JobController controller{*proc, job_interface};
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);

  // Initialize instruction tree and check again
  job_interface.InitializeInstructionTree(proc->RootInstruction());
  EXPECT_EQ(m_test_server_interface.GetSize(), 5);
}

TEST_F(ServerJobInterfaceTest, OnStateChange)
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_EQ(m_test_server_interface.GetSize(), 0);
  ServerJobInterface job_interface{kTestPrefix, *proc, m_test_server_interface};

  // Check variable anyvalues before updates:
  const std::string state_name = GetJobStatePVName(kTestPrefix);
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(state_name), kJobStateAnyValue);

  // Send state update
  job_interface.OnStateChange(sup::sequencer::JobState::kRunning);
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);
  auto state_value = m_test_server_interface.GetAnyValue(state_name);
  ASSERT_TRUE(state_value.HasField(kJobStateField));
  EXPECT_EQ(state_value[kJobStateField].As<sup::dto::uint32>(),
            static_cast<sup::dto::uint32>(sup::sequencer::JobState::kRunning));
}

TEST_F(ServerJobInterfaceTest, VariableUpdated)
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_EQ(m_test_server_interface.GetSize(), 0);
  ServerJobInterface job_interface{kTestPrefix, *proc, m_test_server_interface};

  // Check variable anyvalues before updates:
  const std::string var0_name = GetVariablePVName(kTestPrefix, 0);
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(var0_name), kVariableAnyValue);

  // Send variable update
  sup::dto::AnyValue update{sup::dto::UnsignedInteger64Type, 1729u};
  job_interface.VariableUpdated("one", update, true);
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);
  auto payload =
    sup::protocol::Base64VariableCodec::Decode(m_test_server_interface.GetAnyValue(var0_name));
  ASSERT_TRUE(payload.first);
  ASSERT_TRUE(payload.second.HasField(kVariableValueField));
  EXPECT_EQ(payload.second[kVariableValueField], update);
}

TEST_F(ServerJobInterfaceTest, InstructionUpdates)
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_EQ(m_test_server_interface.GetSize(), 0);
  ServerJobInterface job_interface{kTestPrefix, *proc, m_test_server_interface};

  // Setup and check again
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);
  sup::sequencer::JobController controller{*proc, job_interface};
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);

  // Initialize instruction tree and check again
  auto root_instruction = proc->RootInstruction();
  job_interface.InitializeInstructionTree(root_instruction);
  EXPECT_EQ(m_test_server_interface.GetSize(), 5);

  const std::string instruction_tree_name = GetInstructionTreePVName(kTestPrefix);
  ASSERT_TRUE(m_test_server_interface.HasAnyValue(instruction_tree_name));
  auto instr_tree = m_test_server_interface.GetAnyValue(instruction_tree_name);
  std::string copy0_node_name = kChildrenField + ".Copy0";
  std::string copy1_node_name = kChildrenField + ".Copy1";
  ASSERT_TRUE(instr_tree.HasField(copy0_node_name));
  ASSERT_TRUE(instr_tree.HasField(copy1_node_name));
  auto copy0_node = instr_tree[copy0_node_name];
  auto copy1_node = instr_tree[copy1_node_name];
  ASSERT_TRUE(copy0_node.HasField(kExecStatusField));
  ASSERT_TRUE(copy0_node.HasField(kBreakpointField));
  ASSERT_TRUE(copy0_node.HasField(kChildrenField));
  EXPECT_EQ(copy0_node[kExecStatusField],
    static_cast<sup::dto::uint16>(sup::sequencer::ExecutionStatus::NOT_STARTED));
  EXPECT_EQ(copy0_node[kBreakpointField], false);
  EXPECT_EQ(copy0_node[kChildrenField], sup::dto::EmptyStruct());
  ASSERT_TRUE(copy1_node.HasField(kExecStatusField));
  ASSERT_TRUE(copy1_node.HasField(kBreakpointField));
  ASSERT_TRUE(copy1_node.HasField(kChildrenField));
  EXPECT_EQ(copy1_node[kExecStatusField],
    static_cast<sup::dto::uint16>(sup::sequencer::ExecutionStatus::NOT_STARTED));
  EXPECT_EQ(copy1_node[kBreakpointField], false);
  EXPECT_EQ(copy1_node[kChildrenField], sup::dto::EmptyStruct());

  // Set breakpoint on second copy, start and wait for paused after breakpoint is hit
  ASSERT_EQ(root_instruction->ChildrenCount(), 2);
  auto copy_instruction = root_instruction->ChildInstructions()[1];
  controller.SetBreakpoint(copy_instruction);
  controller.Start();
  const std::string state_name = GetJobStatePVName(kTestPrefix);
  auto paused_state_value = kJobStateAnyValue;
  paused_state_value[kJobStateField] =
    static_cast<sup::dto::uint32>(sup::sequencer::JobState::kPaused);
  ASSERT_TRUE(m_test_server_interface.WaitForValue(state_name, paused_state_value, 1.0));

  // Check again
  ASSERT_TRUE(m_test_server_interface.HasAnyValue(instruction_tree_name));
  instr_tree = m_test_server_interface.GetAnyValue(instruction_tree_name);
  copy0_node = instr_tree[copy0_node_name];
  copy1_node = instr_tree[copy1_node_name];
  ASSERT_TRUE(copy0_node.HasField(kExecStatusField));
  ASSERT_TRUE(copy0_node.HasField(kBreakpointField));
  ASSERT_TRUE(copy0_node.HasField(kChildrenField));
  EXPECT_EQ(copy0_node[kExecStatusField].As<sup::dto::uint16>(),
    static_cast<sup::dto::uint16>(sup::sequencer::ExecutionStatus::SUCCESS));
  EXPECT_EQ(copy0_node[kBreakpointField], false);
  EXPECT_EQ(copy0_node[kChildrenField], sup::dto::EmptyStruct());
  ASSERT_TRUE(copy1_node.HasField(kExecStatusField));
  ASSERT_TRUE(copy1_node.HasField(kBreakpointField));
  ASSERT_TRUE(copy1_node.HasField(kChildrenField));
  EXPECT_EQ(copy1_node[kExecStatusField],
    static_cast<sup::dto::uint16>(sup::sequencer::ExecutionStatus::NOT_STARTED));
  EXPECT_EQ(copy1_node[kBreakpointField], true);
  EXPECT_EQ(copy1_node[kChildrenField], sup::dto::EmptyStruct());

  // std::cout << m_test_server_interface;
}
