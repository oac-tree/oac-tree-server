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

#include <sup/oac-tree-server/websocket/ws_message.h>

#include "ws_message_json_helpers.h"

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
  (void)result.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));
  return result;
}

// Copying variant: stores a copy of the string in the document allocator instead of referencing the
// source buffer. Required when the source string does not outlive serialization, e.g. a temporary
// computed inside a loop.
rapidjson::Value JSONStringValueCopy(const std::string& str,
                                     rapidjson::Document::AllocatorType& allocator)
{
  rapidjson::Value result;
  (void)result.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.size()), allocator);
  return result;
}
}  // unnamed namespace

namespace sup
{
namespace oac_tree_server
{

std::string CreateServerPrefix(const std::string& host_name, unsigned short port)
{
  return host_name + ":" + std::to_string(static_cast<unsigned int>(port));
}

std::string CreateInfoEndpoint(const std::string& server_prefix)
{
  return server_prefix + endpoints::kJobManagerEndpoint;
}

std::string CreateJobEndpoint(const std::string& server_prefix, sup::dto::uint32 job_idx)
{
  return server_prefix + endpoints::kJobEndpointPrefix + std::to_string(job_idx);
}

std::string CreateJobStateUpdateMessage(sup::oac_tree::JobState state)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kJobState),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kState),
                static_cast<sup::dto::uint32>(state),
                allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateBreakpointInstructionUpdateMessage(sup::dto::uint32 instruction_id)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kBreakPoint),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kId), instruction_id, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateInstructionStateUpdateMessage(sup::dto::uint32 instruction_id,
                                                sup::oac_tree::InstructionState state)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kInstructionState),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kId), instruction_id, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kBreakpoint),
                state.m_breakpoint_set, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kState),
                static_cast<sup::dto::uint32>(state.m_execution_status),
                allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateVariableStateUpdateMessage(sup::dto::uint32 variable_id,
                                             bool connected,
                                             const sup::dto::AnyValue& value)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();
  const auto value_str = sup::dto::ValuesToJSONString(value);
  const auto type_str = sup::dto::AnyTypeToJSONString(value.GetType());

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kVariableState),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kId), variable_id, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kConnected), connected, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kValue),
                JSONStringValue(value_str), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kType),
                JSONStringValue(type_str), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreatePutValueUpdateMessage(const std::string& description,
                                        const sup::dto::AnyValue& value)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();
  const auto value_str = sup::dto::ValuesToJSONString(value);
  const auto type_str = sup::dto::AnyTypeToJSONString(value.GetType());

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kPutValue),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kDescription),
                JSONStringValue(description), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kValue),
                JSONStringValue(value_str), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kType),
                JSONStringValue(type_str), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateTextMessageUpdateMessage(const std::string& message)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kMessage),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kMessage),
                JSONStringValue(message), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateLogUpdateMessage(sup::dto::int32 severity,
                                   const std::string& message)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kLog),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kSeverity), severity, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kMessage),
                JSONStringValue(message), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetUserValueUpdateMessage(sup::dto::uint64 request_id,
                                            const std::string& description,
                                            const sup::dto::AnyValue& value)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();
  const auto value_str = sup::dto::ValuesToJSONString(value);
  const auto type_str = sup::dto::AnyTypeToJSONString(value.GetType());

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kGetUserValue),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequestId), request_id, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kDescription),
                JSONStringValue(description), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kValue),
                JSONStringValue(value_str), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kType),
                JSONStringValue(type_str), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetUserChoiceUpdateMessage(sup::dto::uint64 request_id,
                                             const std::vector<std::string>& choices,
                                             const sup::dto::AnyValue& metadata)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();
  const auto value_str = sup::dto::ValuesToJSONString(metadata);
  const auto type_str = sup::dto::AnyTypeToJSONString(metadata.GetType());

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kGetUserChoice),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequestId), request_id, allocator);
  rapidjson::Value choices_array{rapidjson::kArrayType};
  for (const auto& choice : choices)
  {
    (void)choices_array.PushBack(JSONStringValue(choice), allocator);
  }
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kChoices), choices_array, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kValue),
                JSONStringValue(value_str), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kType),
                JSONStringValue(type_str), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateUserInputCancelledMessage(sup::dto::uint64 request_id)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kUserInputCancelled),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequestId), request_id, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

namespace
{
rapidjson::Value CreateSnapshotInstructionEntry(const InstructionStateEntry& entry,
                                                rapidjson::Document::AllocatorType& allocator)
{
  rapidjson::Value instr_obj{rapidjson::kObjectType};
  (void)instr_obj.AddMember(rapidjson::StringRef(ws_message_json_keys::kId),
                entry.id, allocator);
  (void)instr_obj.AddMember(rapidjson::StringRef(ws_message_json_keys::kBreakpoint),
                entry.state.m_breakpoint_set, allocator);
  (void)instr_obj.AddMember(rapidjson::StringRef(ws_message_json_keys::kState),
                static_cast<sup::dto::uint32>(entry.state.m_execution_status), allocator);
  return instr_obj;
}

rapidjson::Value CreateSnapshotVariableEntry(const VariableStateEntry& entry,
                                             rapidjson::Document::AllocatorType& allocator)
{
  const auto value_str = sup::dto::ValuesToJSONString(entry.value);
  const auto type_str = sup::dto::AnyTypeToJSONString(entry.value.GetType());
  rapidjson::Value var_obj{rapidjson::kObjectType};
  (void)var_obj.AddMember(rapidjson::StringRef(ws_message_json_keys::kId),
                entry.id, allocator);
  (void)var_obj.AddMember(rapidjson::StringRef(ws_message_json_keys::kConnected),
                entry.connected, allocator);
  (void)var_obj.AddMember(rapidjson::StringRef(ws_message_json_keys::kValue),
                JSONStringValueCopy(value_str, allocator), allocator);
  (void)var_obj.AddMember(rapidjson::StringRef(ws_message_json_keys::kType),
                JSONStringValueCopy(type_str, allocator), allocator);
  return var_obj;
}

rapidjson::Value CreateSnapshotInstructionArray(
  const std::vector<InstructionStateEntry>& instructions,
  rapidjson::Document::AllocatorType& allocator)
{
  rapidjson::Value instructions_array{rapidjson::kArrayType};
  for (const auto& entry : instructions)
  {
    (void)instructions_array.PushBack(CreateSnapshotInstructionEntry(entry, allocator), allocator);
  }
  return instructions_array;
}

rapidjson::Value CreateSnapshotVariableArray(const std::vector<VariableStateEntry>& variables,
                                             rapidjson::Document::AllocatorType& allocator)
{
  rapidjson::Value variables_array{rapidjson::kArrayType};
  for (const auto& entry : variables)
  {
    (void)variables_array.PushBack(CreateSnapshotVariableEntry(entry, allocator), allocator);
  }
  return variables_array;
}
}  // unnamed namespace

std::string CreateJobSnapshotMessage(const JobSnapshot& snapshot)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kJobSnapshot), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kState),
                static_cast<sup::dto::uint32>(snapshot.job_state), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kId),
                snapshot.current_breakpoint, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kInstructions),
                CreateSnapshotInstructionArray(snapshot.instructions, allocator), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kVariables),
                CreateSnapshotVariableArray(snapshot.variables, allocator), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetUserValueReplyMessage(sup::dto::uint64 request_id, bool ok,
                                           const sup::dto::AnyValue& value)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();
  const auto value_str = sup::dto::ValuesToJSONString(value);
  const auto type_str = sup::dto::AnyTypeToJSONString(value.GetType());

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kGetUserValue),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequestId), request_id, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kOk), ok, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kValue),
                JSONStringValue(value_str), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kType),
                JSONStringValue(type_str), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetUserChoiceReplyMessage(sup::dto::uint64 request_id, int choice)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kUpdate),
                static_cast<sup::dto::uint32>(UpdateMessageType::kGetUserChoice),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequestId), request_id, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kChoice), choice, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetNumberOfJobsRequestMessage(sup::dto::uint64 request_id)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kGetNumberOfJobs),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequestId), request_id, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetJobInfoRequestMessage(sup::dto::uint64 request_id, sup::dto::uint32 job_index)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kGetJobInfo),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequestId), request_id, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kJobIndex), job_index, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateEditBreakpointRequestMessage(sup::dto::uint32 job_index,
                                               sup::dto::uint32 instruction_id,
                                               bool active)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kEditBreakPoint),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kJobIndex), job_index, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kInstructionId), instruction_id, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kActive), active, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateSendJobCommandRequestMessage(sup::dto::uint32 job_index,
                                               sup::oac_tree::JobCommand command)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kSendCommand),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kJobIndex), job_index, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kCommand),
                static_cast<sup::dto::uint32>(command),
                allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetNumberOfJobsReplyMessage(sup::dto::uint64 request_id,
                                              sup::dto::uint32 number_of_jobs)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kGetNumberOfJobs),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequestId), request_id, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kNumberOfJobs), number_of_jobs, allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

std::string CreateGetJobInfoReplyMessage(sup::dto::uint64 request_id, sup::dto::uint32 job_index,
                                         const sup::oac_tree::JobInfo& job_info)
{
  rapidjson::Document doc;
  (void)doc.SetObject();
  auto& allocator = doc.GetAllocator();
  auto job_info_any = sup::oac_tree::utils::ToAnyValue(job_info);
  const auto job_info_val_str = sup::dto::ValuesToJSONString(job_info_any);
  const auto job_info_type_str = sup::dto::AnyTypeToJSONString(job_info_any.GetType());

  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequest),
                static_cast<sup::dto::uint32>(ClientMessageType::kGetJobInfo),
                allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kRequestId), request_id, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kJobIndex), job_index, allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kValue),
                JSONStringValue(job_info_val_str), allocator);
  (void)doc.AddMember(rapidjson::StringRef(ws_message_json_keys::kType),
                JSONStringValue(job_info_type_str), allocator);
  return ws_message_json_helpers::SerializeDocumentToJSON(doc);
}

}  // namespace oac_tree_server

}  // namespace sup
