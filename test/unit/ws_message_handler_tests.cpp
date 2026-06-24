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

#include <sup/oac-tree-server/websocket/ws_message_handler.h>
#include <sup/oac-tree-server/websocket/ws_message.h>
#include <sup/oac-tree-server/websocket/ws_message_json_helpers.h>

#include <sup/oac-tree-server/base/input_request_server.h>

#include <sup/oac-tree/instruction_map.h>
#include <sup/oac-tree/job_info_utils.h>
#include <sup/oac-tree/sequence_parser.h>
#include <sup/oac-tree/user_input_reply.h>

#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue_helper.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

#include <rapidjson/document.h>

#include <chrono>

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
  auto reply = HandleJobManagerMessage(CreateGetNumberOfJobsRequestMessage(42u), m_mock_manager);
  EXPECT_EQ(reply, CreateGetNumberOfJobsReplyMessage(42u, 3u));
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_GetJobInfo)
{
  const auto job_info = BuildJobInfo();
  EXPECT_CALL(m_mock_manager, GetJobInfo(1u)).Times(Exactly(1)).WillOnce(Return(job_info));
  auto reply = HandleJobManagerMessage(CreateGetJobInfoRequestMessage(42u, 1u), m_mock_manager);
  EXPECT_EQ(reply, CreateGetJobInfoReplyMessage(42u, 1u, job_info));
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

// ---------------------------------------------------------------------------
// Robustness: each update handler rejects a message that is missing its fields
// (StrictMock ensures the corresponding IJobInfoIO method is not called)
// ---------------------------------------------------------------------------

TEST_F(UpdateMessageHandlerTest, HandleMessage_Breakpoint_MissingId)
{
  HandleUpdateMessage(R"({"update": 1})", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_InstructionState_MissingFields)
{
  HandleUpdateMessage(R"({"update": 2})", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_VariableState_MissingFields)
{
  HandleUpdateMessage(R"({"update": 3})", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_VariableState_UnparsableValue)
{
  HandleUpdateMessage(
      R"({"update": 3, "id": 1, "connected": true, "value": "x", "type": "not-a-type"})",
      m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_PutValue_MissingFields)
{
  HandleUpdateMessage(R"({"update": 4})", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_TextMessage_MissingMessage)
{
  HandleUpdateMessage(R"({"update": 5})", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_Log_MissingFields)
{
  HandleUpdateMessage(R"({"update": 6})", m_mock_io);
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_GetNumberOfJobs_MissingRequestId)
{
  auto reply = HandleJobManagerMessage(R"({"request": 0})", m_mock_manager);
  EXPECT_TRUE(reply.empty());
}

TEST_F(JobManagerMessageHandlerTest, HandleMessage_GetJobInfo_MissingRequestId)
{
  auto reply = HandleJobManagerMessage(R"({"request": 1, "job_index": 0})", m_mock_manager);
  EXPECT_TRUE(reply.empty());
}

// ===========================================================================
// Reply-parsing free functions
// ===========================================================================

TEST(ReplyParsingTest, ParseNumberOfJobs_Valid)
{
  auto result = ParseNumberOfJobsReply(CreateGetNumberOfJobsReplyMessage(7u, 5u));
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5u);
}

TEST(ReplyParsingTest, ParseNumberOfJobs_InvalidJson)
{
  EXPECT_FALSE(ParseNumberOfJobsReply("not json").has_value());
}

TEST(ReplyParsingTest, ParseNumberOfJobs_MissingField)
{
  EXPECT_FALSE(ParseNumberOfJobsReply(R"({"other": 1})").has_value());
}

TEST(ReplyParsingTest, ParseJobInfo_Valid)
{
  const auto job_info = BuildJobInfo();
  auto result = ParseJobInfoReply(CreateGetJobInfoReplyMessage(7u, 0u, job_info));
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), job_info);
}

TEST(ReplyParsingTest, ParseJobInfo_InvalidJson)
{
  EXPECT_FALSE(ParseJobInfoReply("not json").has_value());
}

TEST(ReplyParsingTest, ParseJobInfo_MissingFields)
{
  EXPECT_FALSE(ParseJobInfoReply(R"({"other": 1})").has_value());
}

TEST(ReplyParsingTest, ParseJobInfo_UnparsableValue)
{
  EXPECT_FALSE(ParseJobInfoReply(R"({"value": "x", "type": "not-a-type"})").has_value());
}

TEST(JobManagerReplyHandlerTest, ValidReplyIsDelegated)
{
  MessageReplyHandler handler;
  auto id_guard = handler.CreateNewMessageId();
  HandleJobManagerReply(CreateGetNumberOfJobsReplyMessage(id_guard.GetId(), 3u), handler);
  auto reply = handler.WaitForReply(id_guard.GetId(), std::chrono::milliseconds(100));
  EXPECT_TRUE(reply.has_value());
}

TEST(JobManagerReplyHandlerTest, InvalidJsonIgnored)
{
  MessageReplyHandler handler;
  HandleJobManagerReply("not json", handler);  // must not throw
  SUCCEED();
}

TEST(JobManagerReplyHandlerTest, MissingRequestIdIgnored)
{
  MessageReplyHandler handler;
  HandleJobManagerReply(R"({"other": 1})", handler);
  SUCCEED();
}

// ===========================================================================
// HandleUserInputReply tests
// ===========================================================================

namespace
{
// Build a "get user value" reply message, as a client would send it back to the server.
std::string CreateUserValueReplyMessage(sup::dto::uint64 req_id, bool ok,
                                        const sup::dto::AnyValue& value)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& alloc = doc.GetAllocator();
  const auto value_str = sup::dto::ValuesToJSONString(value);
  const auto type_str = sup::dto::AnyTypeToJSONString(value.GetType());
  doc.AddMember("update", static_cast<sup::dto::uint32>(UpdateMessageType::kGetUserValue), alloc);
  doc.AddMember("req_id", req_id, alloc);
  doc.AddMember("ok", ok, alloc);
  rapidjson::Value value_member;
  value_member.SetString(value_str.c_str(), alloc);
  rapidjson::Value type_member;
  type_member.SetString(type_str.c_str(), alloc);
  doc.AddMember("value", value_member, alloc);
  doc.AddMember("type", type_member, alloc);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

// Build a "get user choice" reply message, as a client would send it back to the server.
std::string CreateUserChoiceReplyMessage(sup::dto::uint64 req_id, int choice)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& alloc = doc.GetAllocator();
  doc.AddMember("update", static_cast<sup::dto::uint32>(UpdateMessageType::kGetUserChoice), alloc);
  doc.AddMember("req_id", req_id, alloc);
  doc.AddMember("choice", choice, alloc);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}
}  // namespace

TEST(UserInputReplyHandlerTest, ValueReplyDelivered)
{
  InputRequestServer server;
  const sup::dto::uint64 id{33u};
  server.InitNewRequest(id);
  const sup::dto::AnyValue provided{sup::dto::SignedInteger32Type, 7};

  HandleUserInputReply(CreateUserValueReplyMessage(id, true, provided), server);

  auto [retrieved, reply] = server.WaitForReply(id);
  ASSERT_TRUE(retrieved);
  auto [parsed, value] = sup::oac_tree::ParseUserValueReply(reply);
  ASSERT_TRUE(parsed);
  EXPECT_EQ(value, provided);
}

TEST(UserInputReplyHandlerTest, ChoiceReplyDelivered)
{
  InputRequestServer server;
  const sup::dto::uint64 id{34u};
  server.InitNewRequest(id);

  HandleUserInputReply(CreateUserChoiceReplyMessage(id, 1), server);

  auto [retrieved, reply] = server.WaitForReply(id);
  ASSERT_TRUE(retrieved);
  auto [parsed, choice] = sup::oac_tree::ParseUserChoiceReply(reply);
  ASSERT_TRUE(parsed);
  EXPECT_EQ(choice, 1);
}

TEST(UserInputReplyHandlerTest, ChoiceCancelDelivered)
{
  InputRequestServer server;
  const sup::dto::uint64 id{35u};
  server.InitNewRequest(id);

  // A cancel (choice == -1) is still a valid, finished reply.
  HandleUserInputReply(CreateUserChoiceReplyMessage(id, -1), server);

  auto [retrieved, reply] = server.WaitForReply(id);
  ASSERT_TRUE(retrieved);
  auto [parsed, choice] = sup::oac_tree::ParseUserChoiceReply(reply);
  EXPECT_FALSE(parsed);
}

TEST(UserInputReplyHandlerTest, InvalidJsonIgnored)
{
  InputRequestServer server;
  HandleUserInputReply("not json", server);  // must not throw
  SUCCEED();
}

TEST(UserInputReplyHandlerTest, MalformedRepliesIgnored)
{
  InputRequestServer server;
  const sup::dto::uint64 id{36u};
  server.InitNewRequest(id);

  // Object without update/req_id, value reply missing ok/value/type, value reply with unparsable
  // value/type, choice reply missing choice, and a non-user-input update.
  HandleUserInputReply(R"({"foo": 1})", server);
  HandleUserInputReply(R"({"update": 7, "req_id": 36})", server);
  HandleUserInputReply(R"({"update": 7, "req_id": 36, "ok": true, "value": "x", "type": "bad"})",
                       server);
  HandleUserInputReply(R"({"update": 8, "req_id": 36})", server);
  HandleUserInputReply(R"({"update": 4, "req_id": 36})", server);

  // None of the above set a reply, so the only way out of the wait is an interrupt.
  server.Interrupt(id);
  auto result = server.WaitForReply(id);
  EXPECT_FALSE(result.first);
}

// ===========================================================================
// HandleClientJobMessage tests (client-side dispatch)
// ===========================================================================

TEST_F(UpdateMessageHandlerTest, HandleClientJobMessage_ValueRequestDispatched)
{
  const sup::dto::AnyValue value{sup::dto::SignedInteger32Type, 5};
  sup::dto::uint64 seen_id{0};
  sup::dto::AnyValue seen_value{};
  std::string seen_description;

  ClientInputCallbacks callbacks;
  callbacks.on_value_request = [&](sup::dto::uint64 id, const sup::dto::AnyValue& v,
                                   const std::string& description) {
    seen_id = id;
    seen_value = v;
    seen_description = description;
  };
  // StrictMock m_mock_io ensures the request is NOT forwarded as a one-way update.
  HandleClientJobMessage(CreateGetUserValueUpdateMessage(33u, "name", value), m_mock_io, callbacks);

  EXPECT_EQ(seen_id, 33u);
  EXPECT_EQ(seen_value, value);
  EXPECT_EQ(seen_description, "name");
}

TEST_F(UpdateMessageHandlerTest, HandleClientJobMessage_ChoiceRequestDispatched)
{
  const std::vector<std::string> choices{"normal", "degraded", "fault"};
  sup::dto::uint64 seen_id{0};
  std::vector<std::string> seen_options;

  ClientInputCallbacks callbacks;
  callbacks.on_choice_request = [&](sup::dto::uint64 id, const std::vector<std::string>& options,
                                    const sup::dto::AnyValue&) {
    seen_id = id;
    seen_options = options;
  };
  HandleClientJobMessage(CreateGetUserChoiceUpdateMessage(34u, choices, sup::dto::AnyValue{}),
                         m_mock_io, callbacks);

  EXPECT_EQ(seen_id, 34u);
  EXPECT_EQ(seen_options, choices);
}

TEST_F(UpdateMessageHandlerTest, HandleClientJobMessage_CancelledDispatched)
{
  sup::dto::uint64 seen_id{0};
  ClientInputCallbacks callbacks;
  callbacks.on_cancelled = [&](sup::dto::uint64 id) { seen_id = id; };

  HandleClientJobMessage(CreateUserInputCancelledMessage(34u), m_mock_io, callbacks);

  EXPECT_EQ(seen_id, 34u);
}

TEST_F(UpdateMessageHandlerTest, HandleClientJobMessage_OneWayUpdateForwarded)
{
  // A non-user-input update is forwarded to the IJobInfoIO, just like HandleUpdateMessage.
  EXPECT_CALL(m_mock_io, Log(5, std::string("warn"))).Times(Exactly(1));
  HandleClientJobMessage(CreateLogUpdateMessage(5, "warn"), m_mock_io, ClientInputCallbacks{});
}

TEST_F(UpdateMessageHandlerTest, HandleClientJobMessage_MalformedIgnored)
{
  ClientInputCallbacks callbacks;
  bool any_called = false;
  callbacks.on_value_request = [&](sup::dto::uint64, const sup::dto::AnyValue&,
                                   const std::string&) { any_called = true; };
  callbacks.on_choice_request = [&](sup::dto::uint64, const std::vector<std::string>&,
                                    const sup::dto::AnyValue&) { any_called = true; };
  callbacks.on_cancelled = [&](sup::dto::uint64) { any_called = true; };

  HandleClientJobMessage("not json", m_mock_io, callbacks);
  HandleClientJobMessage(R"({"req_id": 1})", m_mock_io, callbacks);               // missing update
  HandleClientJobMessage(R"({"update": 7, "req_id": 1})", m_mock_io, callbacks);  // missing fields
  // Value request with unparsable value/type.
  HandleClientJobMessage(
      R"({"update": 7, "req_id": 1, "description": "d", "value": "x", "type": "bad"})", m_mock_io,
      callbacks);
  HandleClientJobMessage(R"({"update": 8, "req_id": 1})", m_mock_io, callbacks);  // missing choices
  // Choice request with a non-string choices entry.
  HandleClientJobMessage(
      R"({"update": 8, "req_id": 1, "choices": [1], "value": "x", "type": "bad"})", m_mock_io,
      callbacks);
  // Choice request with unparsable metadata value/type.
  HandleClientJobMessage(
      R"({"update": 8, "req_id": 1, "choices": ["a"], "value": "x", "type": "bad"})", m_mock_io,
      callbacks);
  HandleClientJobMessage(R"({"update": 9})", m_mock_io, callbacks);  // cancelled missing req_id

  EXPECT_FALSE(any_called);
}
