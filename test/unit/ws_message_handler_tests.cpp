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

#include <sup/oac-tree-server/ws_message_handler.h>
#include <sup/oac-tree-server/ws_message.h>

#include <sup/oac-tree/instruction_map.h>
#include <sup/oac-tree/job_info_utils.h>
#include <sup/oac-tree/sequence_parser.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

using ::testing::Exactly;
using ::testing::Return;

using namespace sup::oac_tree_server;

using sup::oac_tree::ExecutionStatus;
using sup::oac_tree::InstructionState;
using sup::oac_tree::JobCommand;
using sup::oac_tree::JobState;

class UpdateMessageHandlerTest : public ::testing::Test
{
protected:
  using StrictMockJobInfoIO = ::testing::StrictMock<UnitTestHelper::MockJobInfoIO>;
  StrictMockJobInfoIO m_mock_io;
};

// ---------------------------------------------------------------------------
// Happy-path: one test per concrete handler
// ---------------------------------------------------------------------------

TEST_F(UpdateMessageHandlerTest, HandleMessage_JobStateUpdate)
{
  const auto state = JobState::kFailed;
  EXPECT_CALL(m_mock_io, JobStateUpdated(state)).Times(Exactly(1));
  HandleUpdateMessage(CreateJobStateUpdateMessage(state), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_BreakpointUpdate)
{
  EXPECT_CALL(m_mock_io, BreakpointInstructionUpdated(42u)).Times(Exactly(1));
  HandleUpdateMessage(CreateBreakpointInstructionUpdateMessage(42u), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_InstructionStateUpdate)
{
  InstructionState state{true, ExecutionStatus::SUCCESS};
  EXPECT_CALL(m_mock_io, InstructionStateUpdated(8u, state)).Times(Exactly(1));
  HandleUpdateMessage(CreateInstructionStateUpdateMessage(8u, state), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_VariableStateUpdate)
{
  const sup::dto::AnyValue value{sup::dto::Float64Type, 6.5};
  EXPECT_CALL(m_mock_io, VariableUpdated(7u, value, true)).Times(Exactly(1));
  HandleUpdateMessage(CreateVariableStateUpdateMessage(7u, true, value), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_PutValueUpdate)
{
  const sup::dto::AnyValue value{sup::dto::BooleanType, true};
  EXPECT_CALL(m_mock_io, PutValue(value, std::string("desc"))).Times(Exactly(1));
  HandleUpdateMessage(CreatePutValueUpdateMessage("desc", value), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_TextMessageUpdate)
{
  EXPECT_CALL(m_mock_io, Message(std::string("hello"))).Times(Exactly(1));
  HandleUpdateMessage(CreateTextMessageUpdateMessage("hello"), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_LogUpdate)
{
  EXPECT_CALL(m_mock_io, Log(5, std::string("warn"))).Times(Exactly(1));
  HandleUpdateMessage(CreateLogUpdateMessage(5, "warn"), m_mock_io);
}

// ---------------------------------------------------------------------------
// Robustness: StrictMock ensures no IJobInfoIO method is called
// ---------------------------------------------------------------------------

TEST_F(UpdateMessageHandlerTest, HandleMessage_InvalidJson)
{
  HandleUpdateMessage("not json", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_EmptyString)
{
  HandleUpdateMessage("", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_MissingUpdateField)
{
  HandleUpdateMessage(R"({"state": 0})", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_UnknownUpdateType)
{
  HandleUpdateMessage(R"({"update": 99})", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_JobState_MissingState)
{
  HandleUpdateMessage(R"({"update": 0})", m_mock_io);
}

// ===========================================================================
// HandleJobManagerMessage tests
// ===========================================================================

namespace
{
sup::oac_tree::JobInfo BuildJobInfo()
{
  const auto procedure_string = sup::oac_tree_server::UnitTestHelper::CreateProcedureString(
      kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  auto root = proc->RootInstruction();
  sup::oac_tree::InstructionMap instr_map{root};
  return sup::oac_tree::utils::CreateJobInfo(*proc, instr_map);
}
}  // namespace

class JobManagerMessageHandlerTest : public ::testing::Test
{
protected:
  using StrictMockJobManager = ::testing::StrictMock<UnitTestHelper::MockJobManager>;
  StrictMockJobManager m_mock_manager;
};

// ---------------------------------------------------------------------------
// Happy-path: one test per concrete handler
// ---------------------------------------------------------------------------

TEST_F(JobManagerMessageHandlerTest, HandleMessage_GetNumberOfJobs)
{
  EXPECT_CALL(m_mock_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(3u));
  auto reply = HandleJobManagerMessage(CreateGetNumberOfJobsRequestMessage(), m_mock_manager);
  EXPECT_EQ(reply, CreateGetNumberOfJobsReplyMessage(3u));
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_GetJobInfo)
{
  const auto job_info = BuildJobInfo();
  EXPECT_CALL(m_mock_manager, GetJobInfo(1u)).Times(Exactly(1)).WillOnce(Return(job_info));
  auto reply = HandleJobManagerMessage(CreateGetJobInfoRequestMessage(1u), m_mock_manager);
  EXPECT_EQ(reply, CreateGetJobInfoReplyMessage(1u, job_info));
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_EditBreakpoint)
{
  EXPECT_CALL(m_mock_manager, EditBreakpoint(0u, 5u, true)).Times(Exactly(1));
  auto reply = HandleJobManagerMessage(
      CreateEditBreakpointRequestMessage(0u, 5u, true), m_mock_manager);
  EXPECT_TRUE(reply.empty());
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_SendJobCommand)
{
  EXPECT_CALL(m_mock_manager, SendJobCommand(0u, JobCommand::kStart)).Times(Exactly(1));
  auto reply = HandleJobManagerMessage(
      CreateSendJobCommandRequestMessage(0u, JobCommand::kStart), m_mock_manager);
  EXPECT_TRUE(reply.empty());
}

// ---------------------------------------------------------------------------
// Robustness: StrictMock ensures no IJobManager method is called
// ---------------------------------------------------------------------------

TEST_F(JobManagerMessageHandlerTest, HandleMessage_InvalidJson)
{
  auto reply = HandleJobManagerMessage("not json", m_mock_manager);
  EXPECT_TRUE(reply.empty());
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_EmptyString)
{
  auto reply = HandleJobManagerMessage("", m_mock_manager);
  EXPECT_TRUE(reply.empty());
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_MissingRequestField)
{
  auto reply = HandleJobManagerMessage(R"({"state": 0})", m_mock_manager);
  EXPECT_TRUE(reply.empty());
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_UnknownRequestType)
{
  auto reply = HandleJobManagerMessage(R"({"request": 99})", m_mock_manager);
  EXPECT_TRUE(reply.empty());
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_GetJobInfo_MissingJobIndex)
{
  auto reply = HandleJobManagerMessage(R"({"request": 1})", m_mock_manager);
  EXPECT_TRUE(reply.empty());
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_EditBreakpoint_MissingFields)
{
  auto reply = HandleJobManagerMessage(R"({"request": 2, "job_index": 0, "instr_id": 5})", m_mock_manager);
  EXPECT_TRUE(reply.empty());
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_SendJobCommand_MissingFields)
{
  auto reply = HandleJobManagerMessage(R"({"request": 3, "job_index": 0})", m_mock_manager);
  EXPECT_TRUE(reply.empty());
}
