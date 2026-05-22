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
{};

TEST_F(WsMessageTest, SerializeUpdateMessageFromData)
{
  UpdateMessageData message;
  message.type = UpdateMessageType::kInstructionState;
  message.id = 5u;
  message.breakpoint = true;
  message.state = static_cast<sup::dto::uint32>(sup::oac_tree::ExecutionStatus::SUCCESS);

  EXPECT_EQ(SerializeUpdateMessageJSON(message),
            CreateInstructionStateUpdateMessage(
              5u,
              true,
              {
                false,
                sup::oac_tree::ExecutionStatus::SUCCESS
              }));
}

TEST_F(WsMessageTest, UpdateMessageJSONRoundTrip)
{
  const sup::dto::AnyValue value{ sup::dto::Float64Type, 9.25 };
  auto json = CreateVariableStateUpdateMessage(3u, true, value);

  auto [ok, parsed] = ParseUpdateMessageJSON(json);
  ASSERT_TRUE(ok);
  EXPECT_EQ(parsed.type, UpdateMessageType::kVariableState);
  EXPECT_EQ(parsed.id, 3u);
  EXPECT_TRUE(parsed.connected);
  EXPECT_EQ(parsed.value, value);
}

TEST_F(WsMessageTest, SerializeClientRequestMessageFromData)
{
  ClientRequestMessageData message;
  message.type = ClientMessageType::kEditBreakPoint;
  message.job_index = 2u;
  message.instruction_id = 5u;
  message.active = false;

  EXPECT_EQ(SerializeClientRequestMessageJSON(message),
            CreateEditBreakpointRequestMessage(2u, 5u, false));
}

TEST_F(WsMessageTest, ClientRequestMessageJSONRoundTrip)
{
  auto json = CreateSendJobCommandRequestMessage(2u, sup::oac_tree::JobCommand::kPause);

  auto [ok, parsed] = ParseClientRequestMessageJSON(json);
  ASSERT_TRUE(ok);
  EXPECT_EQ(parsed.type, ClientMessageType::kSendCommand);
  EXPECT_EQ(parsed.job_index, 2u);
  EXPECT_EQ(parsed.command, static_cast<sup::dto::uint32>(sup::oac_tree::JobCommand::kPause));
}

TEST_F(WsMessageTest, SerializeClientReplyMessageFromData)
{
  ClientReplyMessageData message;
  message.type = ClientMessageType::kGetNumberOfJobs;
  message.number_of_jobs = 11u;

  EXPECT_EQ(SerializeClientReplyMessageJSON(message),
            CreateGetNumberOfJobsReplyMessage(11u));
}

TEST_F(WsMessageTest, ClientReplyMessageJSONRoundTrip)
{
  auto json = CreateGetNumberOfJobsReplyMessage(7u);

  auto [ok, parsed] = ParseClientReplyMessageJSON(json);
  ASSERT_TRUE(ok);
  EXPECT_EQ(parsed.type, ClientMessageType::kGetNumberOfJobs);
  EXPECT_EQ(parsed.number_of_jobs, 7u);
}

TEST_F(WsMessageTest, JobInfoReplyParsingNotSupported)
{
  const auto job_info = BuildTestJobInfo();
  auto json = CreateGetJobInfoReplyMessage(job_info);

  auto [ok, parsed] = ParseClientReplyMessageJSON(json);
  (void)parsed;
  EXPECT_FALSE(ok);
}
