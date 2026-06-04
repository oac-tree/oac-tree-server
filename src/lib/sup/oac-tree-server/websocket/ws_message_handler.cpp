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

#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/json_type_parser.h>
#include <sup/dto/json_value_parser.h>

#include <rapidjson/document.h>

#include <functional>
#include <vector>

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
  doc.Parse(msg.c_str());

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

std::string HandleJobManagerMessage(const std::string& msg, IJobManager& job_manager)
{
  rapidjson::Document doc;
  doc.Parse(msg.c_str());

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
  doc.Parse(msg.c_str());

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
  (void)doc;  // Unused parameter
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
