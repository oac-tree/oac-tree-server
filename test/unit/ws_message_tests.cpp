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

#include <rapidjson/document.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree_server;

namespace
{

rapidjson::Document ParseJson(const std::string& json)
{
  rapidjson::Document doc;
  doc.Parse(json.c_str());
  return doc;
}

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

TEST(WsCreateJSONMessageTest, CreateJobStateUpdateMessage)
{
  auto json = CreateJobStateUpdateMessage(sup::oac_tree::JobState::kFailed);
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.IsObject());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kUpdate));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kState));
  EXPECT_EQ(doc[ws_message_json_keys::kUpdate].GetUint(),
            static_cast<sup::dto::uint32>(UpdateMessageType::kJobState));
  EXPECT_EQ(doc[ws_message_json_keys::kState].GetUint(),
            static_cast<sup::dto::uint32>(sup::oac_tree::JobState::kFailed));
}

TEST(WsCreateJSONMessageTest, CreateBreakpointInstructionUpdateMessage)
{
  auto json = CreateBreakpointInstructionUpdateMessage(42u);
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kUpdate));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kId));
  EXPECT_EQ(doc[ws_message_json_keys::kUpdate].GetUint(),
            static_cast<sup::dto::uint32>(UpdateMessageType::kBreakPoint));
  EXPECT_EQ(doc[ws_message_json_keys::kId].GetUint(), 42u);
}

TEST(WsCreateJSONMessageTest, CreateInstructionStateUpdateMessage)
{
  sup::oac_tree::InstructionState state{true, sup::oac_tree::ExecutionStatus::SUCCESS};
  auto json = CreateInstructionStateUpdateMessage(8u, state);
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kUpdate));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kId));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kBreakpoint));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kState));
  EXPECT_EQ(doc[ws_message_json_keys::kUpdate].GetUint(),
            static_cast<sup::dto::uint32>(UpdateMessageType::kInstructionState));
  EXPECT_EQ(doc[ws_message_json_keys::kId].GetUint(), 8u);
  EXPECT_TRUE(doc[ws_message_json_keys::kBreakpoint].GetBool());
  EXPECT_EQ(doc[ws_message_json_keys::kState].GetUint(),
            static_cast<sup::dto::uint32>(sup::oac_tree::ExecutionStatus::SUCCESS));
}

TEST(WsCreateJSONMessageTest, CreateVariableStateUpdateMessage)
{
  const sup::dto::AnyValue value{sup::dto::Float64Type, 6.5};
  auto json = CreateVariableStateUpdateMessage(7u, true, value);
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kUpdate));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kId));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kConnected));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kValue));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kType));
  EXPECT_EQ(doc[ws_message_json_keys::kUpdate].GetUint(),
            static_cast<sup::dto::uint32>(UpdateMessageType::kVariableState));
  EXPECT_EQ(doc[ws_message_json_keys::kId].GetUint(), 7u);
  EXPECT_TRUE(doc[ws_message_json_keys::kConnected].GetBool());
}

TEST(WsCreateJSONMessageTest, CreatePutValueUpdateMessage)
{
  const sup::dto::AnyValue value{sup::dto::BooleanType, true};
  auto json = CreatePutValueUpdateMessage("desc", value);
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kUpdate));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kDescription));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kValue));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kType));
  EXPECT_EQ(doc[ws_message_json_keys::kUpdate].GetUint(),
            static_cast<sup::dto::uint32>(UpdateMessageType::kPutValue));
  EXPECT_STREQ(doc[ws_message_json_keys::kDescription].GetString(), "desc");
}

TEST(WsCreateJSONMessageTest, CreateTextMessageUpdateMessage)
{
  auto json = CreateTextMessageUpdateMessage("hello");
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kUpdate));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kMessage));
  EXPECT_EQ(doc[ws_message_json_keys::kUpdate].GetUint(),
            static_cast<sup::dto::uint32>(UpdateMessageType::kMessage));
  EXPECT_STREQ(doc[ws_message_json_keys::kMessage].GetString(), "hello");
}

TEST(WsCreateJSONMessageTest, CreateLogUpdateMessage)
{
  auto json = CreateLogUpdateMessage(5, "warn");
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kUpdate));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kSeverity));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kMessage));
  EXPECT_EQ(doc[ws_message_json_keys::kUpdate].GetUint(),
            static_cast<sup::dto::uint32>(UpdateMessageType::kLog));
  EXPECT_EQ(doc[ws_message_json_keys::kSeverity].GetInt(), 5);
  EXPECT_STREQ(doc[ws_message_json_keys::kMessage].GetString(), "warn");
}

TEST(WsCreateJSONMessageTest, CreateGetNumberOfJobsRequestMessage)
{
  auto json = CreateGetNumberOfJobsRequestMessage();
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kRequest));
  EXPECT_EQ(doc[ws_message_json_keys::kRequest].GetUint(),
            static_cast<sup::dto::uint32>(ClientMessageType::kGetNumberOfJobs));
}

TEST(WsCreateJSONMessageTest, CreateGetJobInfoRequestMessage)
{
  auto json = CreateGetJobInfoRequestMessage(3u);
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kRequest));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kJobIndex));
  EXPECT_EQ(doc[ws_message_json_keys::kRequest].GetUint(),
            static_cast<sup::dto::uint32>(ClientMessageType::kGetJobInfo));
  EXPECT_EQ(doc[ws_message_json_keys::kJobIndex].GetUint(), 3u);
}

TEST(WsCreateJSONMessageTest, CreateEditBreakpointRequestMessage)
{
  auto json = CreateEditBreakpointRequestMessage(2u, 9u, true);
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kRequest));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kJobIndex));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kInstructionId));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kActive));
  EXPECT_EQ(doc[ws_message_json_keys::kRequest].GetUint(),
            static_cast<sup::dto::uint32>(ClientMessageType::kEditBreakPoint));
  EXPECT_EQ(doc[ws_message_json_keys::kJobIndex].GetUint(), 2u);
  EXPECT_EQ(doc[ws_message_json_keys::kInstructionId].GetUint(), 9u);
  EXPECT_TRUE(doc[ws_message_json_keys::kActive].GetBool());
}

TEST(WsCreateJSONMessageTest, CreateSendJobCommandRequestMessage)
{
  auto json = CreateSendJobCommandRequestMessage(1u, sup::oac_tree::JobCommand::kPause);
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kRequest));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kJobIndex));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kCommand));
  EXPECT_EQ(doc[ws_message_json_keys::kRequest].GetUint(),
            static_cast<sup::dto::uint32>(ClientMessageType::kSendCommand));
  EXPECT_EQ(doc[ws_message_json_keys::kJobIndex].GetUint(), 1u);
  EXPECT_EQ(doc[ws_message_json_keys::kCommand].GetUint(),
            static_cast<sup::dto::uint32>(sup::oac_tree::JobCommand::kPause));
}

TEST(WsCreateJSONMessageTest, CreateGetNumberOfJobsReplyMessage)
{
  auto json = CreateGetNumberOfJobsReplyMessage(11u);
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kRequest));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kNumberOfJobs));
  EXPECT_EQ(doc[ws_message_json_keys::kRequest].GetUint(),
            static_cast<sup::dto::uint32>(ClientMessageType::kGetNumberOfJobs));
  EXPECT_EQ(doc[ws_message_json_keys::kNumberOfJobs].GetUint(), 11u);
}

TEST(WsCreateJSONMessageTest, CreateGetJobInfoReplyMessage)
{
  const auto job_info = BuildTestJobInfo();
  auto json = CreateGetJobInfoReplyMessage(5u, job_info);
  auto doc = ParseJson(json);

  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kRequest));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kJobIndex));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kValue));
  ASSERT_TRUE(doc.HasMember(ws_message_json_keys::kType));
  EXPECT_EQ(doc[ws_message_json_keys::kRequest].GetUint(),
            static_cast<sup::dto::uint32>(ClientMessageType::kGetJobInfo));
}
