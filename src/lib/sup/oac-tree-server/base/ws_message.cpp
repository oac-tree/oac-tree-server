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

#include <sup/oac-tree-server/ws_message.h>
#include <sup/oac-tree-server/base/ws_message_json_helpers.h>
#include <sup/oac-tree/job_info_utils.h>

#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/json_type_parser.h>
#include <sup/dto/json_value_parser.h>

#include <rapidjson/document.h>

namespace
{
rapidjson::Value JSONStringValue(const std::string& str)
{
  rapidjson::Value result;
  result.SetString(rapidjson::StringRef(str));
  return result;
}
}  // unnamed namespace

namespace sup
{
namespace oac_tree_server
{

std::string CreateJobStateUpdateMessage(sup::oac_tree::JobState state)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kJobState),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kState),
                static_cast<sup::dto::uint32>(state),
                allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateBreakpointInstructionUpdateMessage(sup::dto::uint32 instruction_id)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kBreakPoint),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kId), instruction_id, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateInstructionStateUpdateMessage(sup::dto::uint32 instruction_id,
                                                bool breakpoint,
                                                sup::oac_tree::InstructionState state)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kInstructionState),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kId), instruction_id, allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kBreakpoint), breakpoint, allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kState),
                static_cast<sup::dto::uint32>(state.m_execution_status),
                allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateVariableStateUpdateMessage(sup::dto::uint32 variable_id,
                                             bool connected,
                                             const sup::dto::AnyValue& value)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();
  const auto value_str = sup::dto::ValuesToJSONString(value);
  const auto type_str = sup::dto::AnyTypeToJSONString(value.GetType());

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kVariableState),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kId), variable_id, allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kConnected), connected, allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kValue),
                JSONStringValue(value_str), allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kType),
                JSONStringValue(type_str), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreatePutValueUpdateMessage(const std::string& description,
                                        const sup::dto::AnyValue& value)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();
  const auto value_str = sup::dto::ValuesToJSONString(value);
  const auto type_str = sup::dto::AnyTypeToJSONString(value.GetType());

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kPutValue),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kDescription),
                JSONStringValue(description), allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kValue),
                JSONStringValue(value_str), allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kType),
                JSONStringValue(type_str), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateTextMessageUpdateMessage(const std::string& message)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kMessage),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kMessage),
                JSONStringValue(message), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateLogUpdateMessage(sup::dto::int32 severity,
                                   const std::string& message)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kLog),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kSeverity), severity, allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kMessage),
                JSONStringValue(message), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetNumberOfJobsRequestMessage()
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kGetNumberOfJobs),
                allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetJobInfoRequestMessage(sup::dto::uint32 job_index)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kGetJobInfo),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kJobIndex), job_index, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateEditBreakpointRequestMessage(sup::dto::uint32 job_index,
                                               sup::dto::uint32 instruction_id,
                                               bool active)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kEditBreakPoint),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kJobIndex), job_index, allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kInstructionId), instruction_id, allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kActive), active, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateSendJobCommandRequestMessage(sup::dto::uint32 job_index,
                                               sup::oac_tree::JobCommand command)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kSendCommand),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kJobIndex), job_index, allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kCommand),
                static_cast<sup::dto::uint32>(command),
                allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetNumberOfJobsReplyMessage(sup::dto::uint32 number_of_jobs)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kGetNumberOfJobs),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kNumberOfJobs), number_of_jobs, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetJobInfoReplyMessage(const sup::oac_tree::JobInfo& job_info)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();
  auto job_info_any = sup::oac_tree::utils::ToAnyValue(job_info);
  const auto job_info_str = sup::dto::ValuesToJSONString(job_info_any);

  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kGetJobInfo),
                allocator);
  doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kJobInfo),
                JSONStringValue(job_info_str), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

}  // namespace oac_tree_server

}  // namespace sup
