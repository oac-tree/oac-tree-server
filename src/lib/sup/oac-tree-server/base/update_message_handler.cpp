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

#include <sup/oac-tree-server/update_message_handler.h>
#include <sup/oac-tree-server/ws_message.h>

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

using ConcreteMsgHandler = std::function<void(const rapidjson::Document&,
                                              sup::oac_tree::IJobInfoIO&)>;

const std::vector<ConcreteMsgHandler>& GetConcreteHandlers();

void HandleJobStateUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io);
void HandleBreakpointUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io);
void HandleInstructionStateUpdate(const rapidjson::Document& doc,
                                  sup::oac_tree::IJobInfoIO& job_info_io);
void HandleVariableStateUpdate(const rapidjson::Document& doc,
                               sup::oac_tree::IJobInfoIO& job_info_io);
void HandlePutValueUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io);
void HandleMessageUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io);
void HandleLogUpdate(const rapidjson::Document& doc, sup::oac_tree::IJobInfoIO& job_info_io);
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
  auto update_idx = doc[kUpdate].GetUint();
  const auto& concrete_handlers = GetConcreteHandlers();
  if (update_idx >= concrete_handlers.size())
  {
    return;
  }
  concrete_handlers[update_idx](doc, job_info_io);
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

const std::vector<ConcreteMsgHandler>& GetConcreteHandlers()
{
  static const std::vector<ConcreteMsgHandler> concrete_handlers = {
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
}  // unnamed namespace

}  // namespace oac_tree_server

}  // namespace sup
