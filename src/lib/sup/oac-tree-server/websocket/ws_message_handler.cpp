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

#include <sup/oac-tree-server/websocket/ws_message_handler.h>
#include <sup/oac-tree-server/websocket/ws_message.h>

#include <sup/oac-tree-server/exceptions.h>

#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/json_type_parser.h>
#include <sup/dto/json_value_parser.h>
#include <sup/oac-tree/job_info_utils.h>
#include <sup/oac-tree/user_input_reply.h>

#include <rapidjson/document.h>

#include <functional>
#include <vector>

#include <iostream>

namespace sup
{
namespace oac_tree_server
{
using namespace ws_message_json_keys;

namespace
{
bool ParseAnyValue(const rapidjson::Value& value_entry,
                   const rapidjson::Value& type_entry,
                   sup::dto::AnyValue& value);

// Job snapshot parsing helpers.
bool HasValidSnapshotHeader(const rapidjson::Document& doc);
bool ParseSnapshotInstructionEntry(const rapidjson::Value& entry, InstructionStateEntry& result);
bool ParseSnapshotVariableEntry(const rapidjson::Value& entry, VariableStateEntry& result);
bool ParseSnapshotInstructionArray(const rapidjson::Value& array,
                                   std::vector<InstructionStateEntry>& result);
bool ParseSnapshotVariableArray(const rapidjson::Value& array,
                                std::vector<VariableStateEntry>& result);

using ConcreteUpdateHandler = std::function<void(const rapidjson::Document&,
                                              sup::oac_tree::IJobInfoIO&)>;

const std::vector<ConcreteUpdateHandler>& GetConcreteUpdateHandlers();

void HandleJobStateUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io);
void HandleBreakpointUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io);
void HandleInstructionStateUpdate(const rapidjson::Document& doc,
                                  sup::oac_tree::IJobInfoIO& job_info_io);
void HandleVariableStateUpdate(const rapidjson::Document& doc,
                               sup::oac_tree::IJobInfoIO& job_info_io);
void HandlePutValueUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io);
void HandleMessageUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io);
void HandleLogUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io);

// User input dispatch helpers, extracted to keep HandleClientJobMessage / HandleUserInputReply
// simple. Each validates the relevant fields and ignores the message if they are missing.
void DispatchUserValueRequest(const rapidjson::Document& doc, const ClientInputCallbacks& callbacks);
void DispatchUserChoiceRequest(const rapidjson::Document& doc, const ClientInputCallbacks& callbacks);
void DispatchUserInputCancelled(const rapidjson::Document& doc,
                                const ClientInputCallbacks& callbacks);
void HandleUserValueReply(const rapidjson::Document& doc, sup::dto::uint64 request_id,
                          InputRequestServer& request_server);
void HandleUserChoiceReply(const rapidjson::Document& doc, sup::dto::uint64 request_id,
                           InputRequestServer& request_server);
bool HasUserChoiceFields(const rapidjson::Document& doc);
bool ExtractChoices(const rapidjson::Value& choices_array, std::vector<std::string>& options);

using ConcreteRequestHandler = std::function<std::string(const rapidjson::Document&, IJobManager&)>;

const std::vector<ConcreteRequestHandler>& GetConcreteRequestHandlers();

std::string HandleGetNumberOfJobsRequest(const rapidjson::Document& doc, IJobManager& job_manager);
std::string HandleGetJobInfoRequest(const rapidjson::Document& doc, IJobManager& job_manager);
std::string HandleEditBreakpointRequest(const rapidjson::Document& doc, IJobManager& job_manager);
std::string HandleSendJobCommandRequest(const rapidjson::Document& doc, IJobManager& job_manager);

}  // unnamed namespace

void HandleUpdateMessage(const std::string& msg, sup::oac_tree::IJobInfoIO& job_info_io)
{
  rapidjson::Document doc;
  (void)doc.Parse(msg.c_str());

  if (doc.HasParseError() || !doc.IsObject())
  {
    return;
  }
  if (!doc.HasMember(kUpdate) || !doc[kUpdate].IsUint())
  {
    return;
  }
  auto update_type = doc[kUpdate].GetUint();
  const auto& concrete_handlers = GetConcreteUpdateHandlers();
  if (update_type >= concrete_handlers.size())
  {
    return;
  }
  concrete_handlers[update_type](doc, job_info_io);
}

void HandleClientJobMessage(const std::string& msg, sup::oac_tree::IJobInfoIO& job_info_io,
                            const ClientInputCallbacks& callbacks)
{
  rapidjson::Document doc;
  (void)doc.Parse(msg.c_str());

  if (doc.HasParseError() || !doc.IsObject())
  {
    return;
  }
  if (!doc.HasMember(kUpdate) || !doc[kUpdate].IsUint())
  {
    return;
  }
  auto update_type = doc[kUpdate].GetUint();
  switch (static_cast<UpdateMessageType>(update_type))
  {
  case UpdateMessageType::kGetUserValue:
    DispatchUserValueRequest(doc, callbacks);
    break;
  case UpdateMessageType::kGetUserChoice:
    DispatchUserChoiceRequest(doc, callbacks);
    break;
  case UpdateMessageType::kUserInputCancelled:
    DispatchUserInputCancelled(doc, callbacks);
    break;
  default:
  {
    // Any other (one-way) update is forwarded to the IJobInfoIO, just like HandleUpdateMessage.
    const auto& concrete_handlers = GetConcreteUpdateHandlers();
    if (update_type < concrete_handlers.size())
    {
      concrete_handlers[update_type](doc, job_info_io);
    }
    break;
  }
  }
}

std::string HandleJobManagerMessage(const std::string& msg, IJobManager& job_manager)
{
  rapidjson::Document doc;
  (void)doc.Parse(msg.c_str());

  if (doc.HasParseError() || !doc.IsObject())
  {
    return {};
  }
  if (!doc.HasMember(kRequest) || !doc[kRequest].IsUint())
  {
    return {};
  }
  auto request_type = doc[kRequest].GetUint();
  const auto& concrete_handlers = GetConcreteRequestHandlers();
  if (request_type >= concrete_handlers.size())
  {
    return {};
  }
  return concrete_handlers[request_type](doc, job_manager);
}

void HandleJobManagerReply(const std::string& msg, MessageReplyHandler& reply_handler)
{
  rapidjson::Document doc;
  (void)doc.Parse(msg.c_str());

  if (doc.HasParseError() || !doc.IsObject())
  {
    return;
  }
  if (!doc.HasMember(kRequestId) || !doc[kRequestId].IsUint64())
  {
    return;
  }
  auto request_id = doc[kRequestId].GetUint64();
  reply_handler.HandleReply(request_id, msg);
}

void HandleUserInputReply(const std::string& msg, InputRequestServer& request_server)
{
  rapidjson::Document doc;
  (void)doc.Parse(msg.c_str());

  if (doc.HasParseError() || !doc.IsObject())
  {
    return;
  }
  if (!doc.HasMember(kUpdate) || !doc[kUpdate].IsUint()
      || !doc.HasMember(kRequestId) || !doc[kRequestId].IsUint64())
  {
    return;
  }
  auto update_type = static_cast<UpdateMessageType>(doc[kUpdate].GetUint());
  auto request_id = doc[kRequestId].GetUint64();
  if (update_type == UpdateMessageType::kGetUserValue)
  {
    HandleUserValueReply(doc, request_id, request_server);
  }
  else if (update_type == UpdateMessageType::kGetUserChoice)
  {
    HandleUserChoiceReply(doc, request_id, request_server);
  }
  else
  {
    // Other update types are not user input replies and carry no reply to deliver.
  }
}

std::optional<sup::dto::uint32> ParseNumberOfJobsReply(const std::string& reply)
{
  rapidjson::Document doc;
  (void)doc.Parse(reply.c_str());

  if (doc.HasParseError() || !doc.IsObject())
  {
    return std::nullopt;
  }
  if (!doc.HasMember(kNumberOfJobs) || !doc[kNumberOfJobs].IsUint())
  {
    return std::nullopt;
  }
  return doc[kNumberOfJobs].GetUint();
}

std::optional<sup::oac_tree::JobInfo> ParseJobInfoReply(const std::string& reply)
{
  rapidjson::Document doc;
  (void)doc.Parse(reply.c_str());

  if (doc.HasParseError() || !doc.IsObject()
      || !doc.HasMember(kValue) || !doc.HasMember(kType))
  {
    return std::nullopt;
  }
  sup::dto::AnyValue value{};
  if (!ParseAnyValue(doc[kValue], doc[kType], value))
  {
    return std::nullopt;
  }
  try
  {
    return sup::oac_tree::utils::ToJobInfo(value);
  }
  catch(const MessageException& e)
  {
    return std::nullopt;
  }
}

std::optional<JobSnapshot> ParseJobSnapshotMessage(const std::string& msg)
{
  rapidjson::Document doc;
  (void)doc.Parse(msg.c_str());

  if (!HasValidSnapshotHeader(doc))
  {
    return std::nullopt;
  }

  JobSnapshot snapshot{};
  snapshot.job_state = static_cast<sup::oac_tree::JobState>(doc[kState].GetUint());
  snapshot.current_breakpoint = doc[kId].GetUint();
  if (!ParseSnapshotInstructionArray(doc[kInstructions], snapshot.instructions)
      || !ParseSnapshotVariableArray(doc[kVariables], snapshot.variables))
  {
    return std::nullopt;
  }
  return snapshot;
}

namespace
{
bool ParseAnyValue(const rapidjson::Value& value_entry,
                   const rapidjson::Value& type_entry,
                   sup::dto::AnyValue& value)
{
  if (!value_entry.IsString() || !type_entry.IsString())
  {
    return false;
  }
  try
  {
    sup::dto::JSONAnyTypeParser type_parser;
    if (!type_parser.ParseString(type_entry.GetString()))
    {
      return false;
    }
    auto type = type_parser.MoveAnyType();
    sup::dto::JSONAnyValueParser value_parser;
    if (!value_parser.TypedParseString(type, value_entry.GetString()))
    {
      return false;
    }
    value = value_parser.MoveAnyValue();
    return true;
  }
  catch (const std::exception&)
  {
    return false;
  }
}

bool HasValidSnapshotHeader(const rapidjson::Document& doc)
{
  return !doc.HasParseError() && doc.IsObject()
         && doc.HasMember(kState) && doc[kState].IsUint()
         && doc.HasMember(kId) && doc[kId].IsUint()
         && doc.HasMember(kInstructions) && doc[kInstructions].IsArray()
         && doc.HasMember(kVariables) && doc[kVariables].IsArray();
}

bool ParseSnapshotInstructionEntry(const rapidjson::Value& entry, InstructionStateEntry& result)
{
  if (!entry.IsObject()
      || !entry.HasMember(kId) || !entry[kId].IsUint()
      || !entry.HasMember(kBreakpoint) || !entry[kBreakpoint].IsBool()
      || !entry.HasMember(kState) || !entry[kState].IsUint())
  {
    return false;
  }
  result.id = entry[kId].GetUint();
  result.state = sup::oac_tree::InstructionState{
    entry[kBreakpoint].GetBool(),
    static_cast<sup::oac_tree::ExecutionStatus>(entry[kState].GetUint())
  };
  return true;
}

bool ParseSnapshotVariableEntry(const rapidjson::Value& entry, VariableStateEntry& result)
{
  if (!entry.IsObject()
      || !entry.HasMember(kId) || !entry[kId].IsUint()
      || !entry.HasMember(kConnected) || !entry[kConnected].IsBool()
      || !entry.HasMember(kValue) || !entry.HasMember(kType))
  {
    return false;
  }
  sup::dto::AnyValue value{};
  if (!ParseAnyValue(entry[kValue], entry[kType], value))
  {
    return false;
  }
  result.id = entry[kId].GetUint();
  result.connected = entry[kConnected].GetBool();
  result.value = value;
  return true;
}

bool ParseSnapshotInstructionArray(const rapidjson::Value& array,
                                   std::vector<InstructionStateEntry>& result)
{
  for (const auto& entry : array.GetArray())
  {
    InstructionStateEntry instr_entry{};
    if (!ParseSnapshotInstructionEntry(entry, instr_entry))
    {
      return false;
    }
    result.push_back(instr_entry);
  }
  return true;
}

bool ParseSnapshotVariableArray(const rapidjson::Value& array,
                                std::vector<VariableStateEntry>& result)
{
  for (const auto& entry : array.GetArray())
  {
    VariableStateEntry var_entry{};
    if (!ParseSnapshotVariableEntry(entry, var_entry))
    {
      return false;
    }
    result.push_back(var_entry);
  }
  return true;
}

const std::vector<ConcreteUpdateHandler>& GetConcreteUpdateHandlers()
{
  static const std::vector<ConcreteUpdateHandler> concrete_handlers = {
    &HandleJobStateUpdate,
    &HandleBreakpointUpdate,
    &HandleInstructionStateUpdate,
    &HandleVariableStateUpdate,
    &HandlePutValueUpdate,
    &HandleMessageUpdate,
    &HandleLogUpdate
  };
  return concrete_handlers;
}

void HandleJobStateUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io)
{
  if (!doc.HasMember(kState) || !doc[kState].IsUint())
  {
    return;
  }
  auto state = static_cast<sup::oac_tree::JobState>(doc[kState].GetUint());
  job_info_io.JobStateUpdated(state);
  return;
}

void HandleBreakpointUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io)
{
  if (!doc.HasMember(kId) || !doc[kId].IsUint())
  {
    return;
  }
  job_info_io.BreakpointInstructionUpdated(doc[kId].GetUint());
  return;
}

void HandleInstructionStateUpdate(const rapidjson::Document& doc,
                                  sup::oac_tree::IJobInfoIO& job_info_io)
{
  if (!doc.HasMember(kId) || !doc[kId].IsUint()
      || !doc.HasMember(kBreakpoint) || !doc[kBreakpoint].IsBool()
      || !doc.HasMember(kState) || !doc[kState].IsUint())
  {
    return;
  }

  auto state = sup::oac_tree::InstructionState{
    doc[kBreakpoint].GetBool(),
    static_cast<sup::oac_tree::ExecutionStatus>(doc[kState].GetUint())
  };
  job_info_io.InstructionStateUpdated(doc[kId].GetUint(), state);
  return;
}

void HandleVariableStateUpdate(const rapidjson::Document& doc,
                               sup::oac_tree::IJobInfoIO& job_info_io)
{
  if (!doc.HasMember(kId) || !doc[kId].IsUint()
      || !doc.HasMember(kConnected) || !doc[kConnected].IsBool()
      || !doc.HasMember(kValue) || !doc.HasMember(kType))
  {
    return;
  }

  sup::dto::AnyValue value{};
  if (!ParseAnyValue(doc[kValue], doc[kType], value))
  {
    return;
  }
  job_info_io.VariableUpdated(doc[kId].GetUint(), value, doc[kConnected].GetBool());
  return;
}

void HandlePutValueUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io)
{
  if (!doc.HasMember(kDescription) || !doc[kDescription].IsString()
      || !doc.HasMember(kValue) || !doc.HasMember(kType))
  {
    return;
  }

  sup::dto::AnyValue value{};
  if (!ParseAnyValue(doc[kValue], doc[kType], value))
  {
    return;
  }
  job_info_io.PutValue(value, doc[kDescription].GetString());
  return;
}

void HandleMessageUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io)
{
  if (!doc.HasMember(kMessage) || !doc[kMessage].IsString())
  {
    return;
  }
  job_info_io.Message(doc[kMessage].GetString());
  return;
}

void HandleLogUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io)
{
  if (!doc.HasMember(kSeverity) || !doc[kSeverity].IsInt()
      || !doc.HasMember(kMessage) || !doc[kMessage].IsString())
  {
    return;
  }
  job_info_io.Log(doc[kSeverity].GetInt(), doc[kMessage].GetString());
  return;
}

void DispatchUserValueRequest(const rapidjson::Document& doc, const ClientInputCallbacks& callbacks)
{
  if (!doc.HasMember(kRequestId) || !doc[kRequestId].IsUint64()
      || !doc.HasMember(kDescription) || !doc[kDescription].IsString()
      || !doc.HasMember(kValue) || !doc.HasMember(kType))
  {
    return;
  }
  sup::dto::AnyValue value{};
  if (!ParseAnyValue(doc[kValue], doc[kType], value))
  {
    return;
  }
  if (callbacks.on_value_request)
  {
    callbacks.on_value_request(doc[kRequestId].GetUint64(), value, doc[kDescription].GetString());
  }
}

bool HasUserChoiceFields(const rapidjson::Document& doc)
{
  return doc.HasMember(kRequestId) && doc[kRequestId].IsUint64()
         && doc.HasMember(kChoices) && doc[kChoices].IsArray()
         && doc.HasMember(kValue) && doc.HasMember(kType);
}

bool ExtractChoices(const rapidjson::Value& choices_array, std::vector<std::string>& options)
{
  for (const auto& choice : choices_array.GetArray())
  {
    if (!choice.IsString())
    {
      return false;
    }
    options.push_back(choice.GetString());
  }
  return true;
}

void DispatchUserChoiceRequest(const rapidjson::Document& doc, const ClientInputCallbacks& callbacks)
{
  if (!HasUserChoiceFields(doc))
  {
    return;
  }
  std::vector<std::string> options;
  if (!ExtractChoices(doc[kChoices], options))
  {
    return;
  }
  sup::dto::AnyValue metadata{};
  if (!ParseAnyValue(doc[kValue], doc[kType], metadata))
  {
    return;
  }
  if (callbacks.on_choice_request)
  {
    callbacks.on_choice_request(doc[kRequestId].GetUint64(), options, metadata);
  }
}

void DispatchUserInputCancelled(const rapidjson::Document& doc,
                                const ClientInputCallbacks& callbacks)
{
  if (!doc.HasMember(kRequestId) || !doc[kRequestId].IsUint64())
  {
    return;
  }
  if (callbacks.on_cancelled)
  {
    callbacks.on_cancelled(doc[kRequestId].GetUint64());
  }
}

void HandleUserValueReply(const rapidjson::Document& doc, sup::dto::uint64 request_id,
                          InputRequestServer& request_server)
{
  if (!doc.HasMember(kOk) || !doc[kOk].IsBool()
      || !doc.HasMember(kValue) || !doc.HasMember(kType))
  {
    return;
  }
  sup::dto::AnyValue value{};
  if (!ParseAnyValue(doc[kValue], doc[kType], value))
  {
    return;
  }
  auto reply = sup::oac_tree::CreateUserValueReply(doc[kOk].GetBool(), value);
  // A stale or duplicate reply (id mismatch / already answered) is intentionally ignored.
  (void)request_server.SetClientReply(request_id, reply);
}

void HandleUserChoiceReply(const rapidjson::Document& doc, sup::dto::uint64 request_id,
                           InputRequestServer& request_server)
{
  if (!doc.HasMember(kChoice) || !doc[kChoice].IsInt())
  {
    return;
  }
  auto choice = doc[kChoice].GetInt();
  auto reply = sup::oac_tree::CreateUserChoiceReply(choice >= 0, choice);
  // A stale or duplicate reply (id mismatch / already answered) is intentionally ignored.
  (void)request_server.SetClientReply(request_id, reply);
}

const std::vector<ConcreteRequestHandler>& GetConcreteRequestHandlers()
{
  static const std::vector<ConcreteRequestHandler> concrete_handlers = {
    &HandleGetNumberOfJobsRequest,
    &HandleGetJobInfoRequest,
    &HandleEditBreakpointRequest,
    &HandleSendJobCommandRequest
  };
  return concrete_handlers;
}

std::string HandleGetNumberOfJobsRequest(const rapidjson::Document& doc, IJobManager& job_manager)
{
  if (!doc.HasMember(kRequestId) || !doc[kRequestId].IsUint64())
  {
    return {};
  }
  auto request_id = doc[kRequestId].GetUint64();
  auto number_of_jobs = job_manager.GetNumberOfJobs();
  return CreateGetNumberOfJobsReplyMessage(request_id, number_of_jobs);
}

std::string HandleGetJobInfoRequest(const rapidjson::Document& doc, IJobManager& job_manager)
{
  if (!doc.HasMember(kJobIndex) || !doc[kJobIndex].IsUint())
  {
    return {};
  }
  if (!doc.HasMember(kRequestId) || !doc[kRequestId].IsUint64())
  {
    return {};
  }
  auto request_id = doc[kRequestId].GetUint64();
  auto job_index = doc[kJobIndex].GetUint();
  auto job_info = job_manager.GetJobInfo(job_index);
  return CreateGetJobInfoReplyMessage(request_id, job_index, job_info);
}

std::string HandleEditBreakpointRequest(const rapidjson::Document& doc, IJobManager& job_manager)
{
  if (!doc.HasMember(kJobIndex) || !doc[kJobIndex].IsUint()
      || !doc.HasMember(kInstructionId) || !doc[kInstructionId].IsUint()
      || !doc.HasMember(kActive) || !doc[kActive].IsBool())
  {
    return {};
  }
  auto job_index = doc[kJobIndex].GetUint();
  auto instruction_id = doc[kInstructionId].GetUint();
  auto breakpoint = doc[kActive].GetBool();
  job_manager.EditBreakpoint(job_index, instruction_id, breakpoint);
  return {};
}

std::string HandleSendJobCommandRequest(const rapidjson::Document& doc, IJobManager& job_manager)
{
  if (!doc.HasMember(kJobIndex) || !doc[kJobIndex].IsUint()
      || !doc.HasMember(kCommand) || !doc[kCommand].IsUint())
  {
    return {};
  }
  auto job_index = doc[kJobIndex].GetUint();
  auto command = static_cast<sup::oac_tree::JobCommand>(doc[kCommand].GetUint());
  job_manager.SendJobCommand(job_index, command);
  return {};
}

}  // unnamed namespace

}  // namespace oac_tree_server

}  // namespace sup
