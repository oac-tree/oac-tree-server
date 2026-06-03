/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : Unit test code
 *
 * Author        : Marcelo Mariano (External)
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

#include "unit_test_helper.h"

#include <sup/oac-tree-server/base/control_protocol_server.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>
#include <sup/oac-tree/instruction_map.h>
#include <sup/oac-tree/job_info_utils.h>
#include <sup/oac-tree/sequence_parser.h>
#include <sup/protocol/function_protocol_pack.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Exactly;
using ::testing::Return;

using namespace sup::oac_tree_server;

class ControlProtocolServerTest : public ::testing::Test
{
protected:
  ControlProtocolServerTest() : m_server{m_job_manager} {}

  ::testing::StrictMock<UnitTestHelper::MockJobManager> m_job_manager;
  ControlProtocolServer m_server;
};

/**
 * @brief Test EditBreakpoint with invalid job index
 */
TEST_F(ControlProtocolServerTest, EditBreakpointInvalidJobIndex)
{
  sup::dto::AnyValue input =
      sup::protocol::FunctionProtocolInput(kEditBreakpointCommandFunctionName);
  sup::protocol::FunctionProtocolPack(input, kJobIndexFieldName,
                                      sup::dto::AnyValue{sup::dto::UnsignedInteger64Type, 10u});
  sup::dto::AnyValue output;

  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(5u));

  EXPECT_EQ(m_server.Invoke(input, output), UnknownJob);
}

/**
 * @brief Test EditBreakpoint with invalid instruction index
 */
TEST_F(ControlProtocolServerTest, EditBreakpointInvalidInstructionIndex)
{
  sup::dto::AnyValue input =
      sup::protocol::FunctionProtocolInput(kEditBreakpointCommandFunctionName);
  sup::protocol::FunctionProtocolPack(input, kJobIndexFieldName,
                                      sup::dto::AnyValue{sup::dto::UnsignedInteger64Type, 0u});
  sup::protocol::FunctionProtocolPack(input, kInstructionIndexFieldName,
                                      sup::dto::AnyValue{sup::dto::UnsignedInteger64Type, 999u});
  sup::dto::AnyValue output;

  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  auto root = proc->RootInstruction();
  sup::oac_tree::InstructionMap instr_map{root};
  auto job_info = sup::oac_tree::utils::CreateJobInfo(*proc, instr_map);

  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(5u));
  EXPECT_CALL(m_job_manager, GetJobInfo(0u)).Times(Exactly(1)).WillOnce(Return(job_info));

  EXPECT_EQ(m_server.Invoke(input, output), UnknownInstruction);
}

/**
 * @brief Test EditBreakpoint with missing breakpoint_active field
 */
TEST_F(ControlProtocolServerTest, EditBreakpointMissingBreakpointActive)
{
  sup::dto::AnyValue input =
      sup::protocol::FunctionProtocolInput(kEditBreakpointCommandFunctionName);
  sup::protocol::FunctionProtocolPack(input, kJobIndexFieldName,
                                      sup::dto::AnyValue{sup::dto::UnsignedInteger64Type, 0u});
  sup::protocol::FunctionProtocolPack(input, kInstructionIndexFieldName,
                                      sup::dto::AnyValue{sup::dto::UnsignedInteger64Type, 1u});
  sup::dto::AnyValue output;

  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  auto root = proc->RootInstruction();
  sup::oac_tree::InstructionMap instr_map{root};
  auto job_info = sup::oac_tree::utils::CreateJobInfo(*proc, instr_map);

  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(5u));
  EXPECT_CALL(m_job_manager, GetJobInfo(0u)).Times(Exactly(1)).WillOnce(Return(job_info));

  EXPECT_EQ(m_server.Invoke(input, output), sup::protocol::ServerProtocolDecodingError);
}

/**
 * @brief Test SendJobCommand with invalid job index
 */
TEST_F(ControlProtocolServerTest, SendJobCommandInvalidJobIndex)
{
  sup::dto::AnyValue input = sup::protocol::FunctionProtocolInput(kSendJobCommandFunctionName);
  sup::protocol::FunctionProtocolPack(input, kJobIndexFieldName,
                                      sup::dto::AnyValue{sup::dto::UnsignedInteger64Type, 10u});
  sup::dto::AnyValue output;

  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(5u));

  EXPECT_EQ(m_server.Invoke(input, output), UnknownJob);
}

/**
 * @brief Test SendJobCommand with invalid job command
 */
TEST_F(ControlProtocolServerTest, SendJobCommandInvalidCommand)
{
  sup::dto::AnyValue input = sup::protocol::FunctionProtocolInput(kSendJobCommandFunctionName);
  sup::protocol::FunctionProtocolPack(input, kJobIndexFieldName,
                                      sup::dto::AnyValue{sup::dto::UnsignedInteger64Type, 0u});
  sup::protocol::FunctionProtocolPack(input, kJobCommandFieldName,
                                      sup::dto::AnyValue{sup::dto::UnsignedInteger32Type, 999u});
  sup::dto::AnyValue output;

  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(5u));

  EXPECT_EQ(m_server.Invoke(input, output), UnknownJobCommand);
}
