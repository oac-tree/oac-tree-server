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

namespace sup
{
namespace oac_tree_server
{
using namespace ws_message_json_keys;

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
}  // unnamed namespace

UpdateMessageHandler::UpdateMessageHandler(sup::oac_tree::IJobInfoIO& job_info_io)
  : m_job_info_io{job_info_io}
{}

UpdateMessageHandler::~UpdateMessageHandler() = default;

void UpdateMessageHandler::HandleMessage(const std::string& msg)
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

  auto update_type = static_cast<UpdateMessageType>(doc[kUpdate].GetUint());
  switch (update_type)
  {
  case UpdateMessageType::kJobState:
    {
      if (!doc.HasMember(kState) || !doc[kState].IsUint())
      {
        return;
      }
      auto state = static_cast<sup::oac_tree::JobState>(doc[kState].GetUint());
      m_job_info_io.JobStateUpdated(state);
      return;
    }
  case UpdateMessageType::kBreakPoint:
    {
      if (!doc.HasMember(kId) || !doc[kId].IsUint())
      {
        return;
      }
      m_job_info_io.BreakpointInstructionUpdated(doc[kId].GetUint());
      return;
    }
  case UpdateMessageType::kInstructionState:
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
      m_job_info_io.InstructionStateUpdated(doc[kId].GetUint(), state);
      return;
    }
  case UpdateMessageType::kVariableState:
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
      m_job_info_io.VariableUpdated(doc[kId].GetUint(), value, doc[kConnected].GetBool());
      return;
    }
  case UpdateMessageType::kPutValue:
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
      m_job_info_io.PutValue(value, doc[kDescription].GetString());
      return;
    }
  case UpdateMessageType::kMessage:
    {
      if (!doc.HasMember(kMessage) || !doc[kMessage].IsString())
      {
        return;
      }
      m_job_info_io.Message(doc[kMessage].GetString());
      return;
    }
  case UpdateMessageType::kLog:
    {
      if (!doc.HasMember(kSeverity) || !doc[kSeverity].IsInt()
          || !doc.HasMember(kMessage) || !doc[kMessage].IsString())
      {
        return;
      }
      m_job_info_io.Log(doc[kSeverity].GetInt(), doc[kMessage].GetString());
      return;
    }
  default:
    return;
  }
}

}  // namespace oac_tree_server

}  // namespace sup
