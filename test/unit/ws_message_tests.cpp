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

#include <sup/oac-tree-server/ws_message.h>

#include <sup/oac-tree/instruction_map.h>
#include <sup/oac-tree/job_info_utils.h>
#include <sup/oac-tree/sequence_parser.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

using namespace sup::oac_tree_server;

namespace
{

sup::oac_tree::JobInfo BuildTestJobInfo()
{
  const auto procedure_string = sup::oac_tree_server::UnitTestHelper::CreateProcedureString(
      kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  EXPECT_NE(proc.get(), nullptr);
  auto root = proc->RootInstruction();
  sup::oac_tree::InstructionMap instr_map{root};
  return sup::oac_tree::utils::CreateJobInfo(*proc, instr_map);
}

}  // namespace

class WsMessageTest : public ::testing::Test
{
protected:
  WsMessageTest() = default;
  virtual ~WsMessageTest() = default;
};

TEST_F(WsMessageTest, UpdateMessagesExposeTypeAndPayload)
{
  const UpdateMessage base_update{UpdateMessageType::kLog};
  EXPECT_EQ(base_update.GetType(), UpdateMessageType::kLog);

  const JobStateUpdateMessage job_state{sup::oac_tree::JobState::kFailed};
  EXPECT_EQ(job_state.GetType(), UpdateMessageType::kJobState);
  EXPECT_EQ(job_state.GetState(), sup::oac_tree::JobState::kFailed);

  const BreakpointInstructionUpdateMessage breakpoint{42u};
  EXPECT_EQ(breakpoint.GetType(), UpdateMessageType::kBreakPoint);
  EXPECT_EQ(breakpoint.GetInstructionId(), 42u);

    const sup::oac_tree::InstructionState expected_state{
      false,
      sup::oac_tree::ExecutionStatus::SUCCESS
    };
    const InstructionStateUpdateMessage instruction_state{5u, true, expected_state};
  EXPECT_EQ(instruction_state.GetType(), UpdateMessageType::kInstructionState);
  EXPECT_EQ(instruction_state.GetInstructionId(), 5u);
  EXPECT_TRUE(instruction_state.IsBreakpoint());
  EXPECT_EQ(instruction_state.GetState(), expected_state);

  const sup::dto::AnyValue variable_value{ sup::dto::BooleanType, true };
  const VariableStateUpdateMessage variable_state{17u, true, variable_value};
  EXPECT_EQ(variable_state.GetType(), UpdateMessageType::kVariableState);
  EXPECT_EQ(variable_state.GetVariableId(), 17u);
  EXPECT_TRUE(variable_state.IsConnected());
  EXPECT_EQ(variable_state.GetValue(), variable_value);

  const sup::dto::AnyValue output_value{ sup::dto::Float64Type, 3.14 };
  const PutValueUpdateMessage put_value{"test output", output_value};
  EXPECT_EQ(put_value.GetType(), UpdateMessageType::kPutValue);
  EXPECT_EQ(put_value.GetDescription(), "test output");
  EXPECT_EQ(put_value.GetValue(), output_value);

  const TextMessageUpdateMessage text{"hello"};
  EXPECT_EQ(text.GetType(), UpdateMessageType::kMessage);
  EXPECT_EQ(text.GetMessage(), "hello");

  const LogUpdateMessage log{7, "warning"};
  EXPECT_EQ(log.GetType(), UpdateMessageType::kLog);
  EXPECT_EQ(log.GetSeverity(), 7);
  EXPECT_EQ(log.GetMessage(), "warning");
}

TEST_F(WsMessageTest, ClientRequestMessagesExposeTypeAndPayload)
{
  const ClientRequestMessage base_request{ClientMessageType::kSendCommand};
  EXPECT_EQ(base_request.GetType(), ClientMessageType::kSendCommand);

  const GetNumberOfJobsRequestMessage get_number_of_jobs;
  EXPECT_EQ(get_number_of_jobs.GetType(), ClientMessageType::kGetNumberOfJobs);

  const GetJobInfoRequestMessage get_job_info{3u};
  EXPECT_EQ(get_job_info.GetType(), ClientMessageType::kGetJobInfo);
  EXPECT_EQ(get_job_info.GetJobIndex(), 3u);

  const EditBreakpointRequestMessage edit_breakpoint{2u, 9u, true};
  EXPECT_EQ(edit_breakpoint.GetType(), ClientMessageType::kEditBreakPoint);
  EXPECT_EQ(edit_breakpoint.GetJobIndex(), 2u);
  EXPECT_EQ(edit_breakpoint.GetInstructionId(), 9u);
  EXPECT_TRUE(edit_breakpoint.IsActive());

  const SendJobCommandRequestMessage send_command{1u, sup::oac_tree::JobCommand::kPause};
  EXPECT_EQ(send_command.GetType(), ClientMessageType::kSendCommand);
  EXPECT_EQ(send_command.GetJobIndex(), 1u);
  EXPECT_EQ(send_command.GetCommand(), sup::oac_tree::JobCommand::kPause);
}

TEST_F(WsMessageTest, ClientReplyMessagesExposeTypeAndPayload)
{
  const ClientReplyMessage base_reply{ClientMessageType::kGetJobInfo};
  EXPECT_EQ(base_reply.GetType(), ClientMessageType::kGetJobInfo);

  const GetNumberOfJobsReplyMessage number_of_jobs_reply{11u};
  EXPECT_EQ(number_of_jobs_reply.GetType(), ClientMessageType::kGetNumberOfJobs);
  EXPECT_EQ(number_of_jobs_reply.GetNumberOfJobs(), 11u);

  const auto job_info = BuildTestJobInfo();
  const GetJobInfoReplyMessage job_info_reply{job_info};
  EXPECT_EQ(job_info_reply.GetType(), ClientMessageType::kGetJobInfo);
  EXPECT_EQ(job_info_reply.GetJobInfo(), job_info);
}
