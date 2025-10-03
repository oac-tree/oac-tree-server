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
 * Copyright (c) : 2010-2025 ITER Organization,
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

#include <sup/oac-tree-server/client_anyvalue_manager.h>

#include <sup/oac-tree-server/output_entry_helper.h>
#include <sup/oac-tree-server/output_entry_types.h>

#include <sup/oac-tree/anyvalue_utils.h>
#include <sup/oac-tree/user_input_reply.h>

namespace
{
using namespace sup::oac_tree_server;
using sup::oac_tree::IJobInfoIO;

void UpdateJobState(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue);
void UpdateInstructionState(IJobInfoIO& job_info_io, sup::dto::uint32 instr_idx,
                            const sup::dto::AnyValue& anyvalue);
void UpdateVariableState(IJobInfoIO& job_info_io, sup::dto::uint32 var_idx,
                        const sup::dto::AnyValue& anyvalue);
void UpdateLogEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue);
void UpdateMessageEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue);
void UpdateOutputValueEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue);
void UpdateBreakpointInstruction(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue);
}  // unnamed namespace

namespace sup
{
namespace oac_tree_server
{

using namespace sup::oac_tree;

ClientAnyValueManager::ClientAnyValueManager(IJobInfoIO& job_info_io)
  : m_job_info_io{job_info_io}
  , m_cb_map{}
{}

ClientAnyValueManager::~ClientAnyValueManager() = default;

bool ClientAnyValueManager::AddAnyValues(const NameAnyValueSet& name_value_set)
{
  sup::dto::uint32 n_instr = 0;
  for (auto& [name, value] : name_value_set)
  {
    if (m_cb_map.find(name) != m_cb_map.end())
    {
      return false;
    }
    auto value_name_info = ParseValueName(name);
    if (value_name_info.val_type == ValueNameType::kInstruction)
    {
      ++n_instr;
    }
    auto cb = CreateCallback(value_name_info);
    cb(m_job_info_io, value);
    m_cb_map[name] = cb;
  }
  if (n_instr > 0)
  {
    m_job_info_io.InitNumberOfInstructions(n_instr);
  }
  return true;
}

bool ClientAnyValueManager::AddInputHandler(const std::string& input_server_name)
{
  (void)input_server_name;
  return true;
}

bool ClientAnyValueManager::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  auto iter = m_cb_map.find(name);
  if (iter == m_cb_map.end())
  {
    return false;
  }
  iter->second(m_job_info_io, value);
  return true;
}

UserInputReply ClientAnyValueManager::GetUserInput(
  const std::string& input_server_name, sup::dto::uint64 id, const UserInputRequest& request)
{
  (void)input_server_name;
  switch (request.m_request_type)
  {
  case InputRequestType::kUserValue:
    {
      std::string description;
      sup::dto::AnyValue value;
      if (!ParseUserValueRequest(request, value, description))
      {
        break;
      }
      auto result = m_job_info_io.GetUserValue(id, value, description);
      return CreateUserValueReply(result, value);
    }
  case InputRequestType::kUserChoice:
    {
      std::vector<std::string> options{};
      sup::dto::AnyValue metadata{};
      if (ParseUserChoiceRequest(request, options, metadata))
      {
        auto choice = m_job_info_io.GetUserChoice(id, options, metadata);
        return CreateUserChoiceReply(true, choice);
      }
      break;
    }
  default:
    break;
  }
  return kInvalidUserInputReply;
}

void ClientAnyValueManager::Interrupt(const std::string& input_server_name, sup::dto::uint64 id)
{
  (void)input_server_name;
  m_job_info_io.Interrupt(id);
}

ClientAnyValueManager::AnyValueCallback CreateCallback(const ValueNameInfo& value_name_info)
{
  auto idx = value_name_info.idx;
  switch (value_name_info.val_type)
  {
  case ValueNameType::kJobStatus:
    return UpdateJobState;
  case ValueNameType::kInstruction:
    {
      auto callback = [idx](IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue) {
        return UpdateInstructionState(job_info_io, idx, anyvalue);
      };
      return callback;
    }
  case ValueNameType::kVariable:
    {
      auto callback = [idx](IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue) {
        return UpdateVariableState(job_info_io, idx, anyvalue);
      };
      return callback;
    }
  case ValueNameType::kLogEntry:
    return UpdateLogEntry;
  case ValueNameType::kMessageEntry:
    return UpdateMessageEntry;
  case ValueNameType::kOutputValueEntry:
    return UpdateOutputValueEntry;
  case ValueNameType::kBreakpointInstruction:
    return UpdateBreakpointInstruction;
  case ValueNameType::kUnknown:
    break;
  default:
    break;
  }
  auto ignore = [](IJobInfoIO& job_info_io, const sup::dto::AnyValue& value) {
    (void)job_info_io;
    (void)value;
    return;
  };
  return ignore;
}

}  // namespace oac_tree_server

}  // namespace sup

namespace
{
void UpdateJobState(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue)
{
  if (!sup::oac_tree::utils::ValidateMemberType(anyvalue, kJobStateField,
                                                 sup::dto::UnsignedInteger32Type))
  {
    return;
  }
  auto job_state_int = anyvalue[kJobStateField].As<sup::dto::uint32>();
  auto job_state = static_cast<sup::oac_tree::JobState>(job_state_int);
  job_info_io.JobStateUpdated(job_state);
}

void UpdateInstructionState(IJobInfoIO& job_info_io, sup::dto::uint32 instr_idx,
                            const sup::dto::AnyValue& anyvalue)
{
  try
  {
    auto instr_state = sup::oac_tree::ToInstructionState(anyvalue);
    job_info_io.InstructionStateUpdated(instr_idx, instr_state);
  }
  catch(const std::exception&)
  {
    // Ignore wrong encoding of AnyValue
  }
  return;
}

void UpdateVariableState(IJobInfoIO& job_info_io, sup::dto::uint32 var_idx,
                        const sup::dto::AnyValue& anyvalue)
{
  auto [value, state] = DecodeVariableState(anyvalue);
  if (sup::dto::IsEmptyValue(value))
  {
    return;
  }
  job_info_io.VariableUpdated(var_idx, value, state);
}

void UpdateLogEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue)
{
  auto [valid, log_entry] = DecodeLogEntry(anyvalue);
  if (valid)
  {
    job_info_io.Log(log_entry.m_severity, log_entry.m_message);
  }
}

void UpdateMessageEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue)
{
  auto [valid, msg_entry] = DecodeMessageEntry(anyvalue);
  if (valid)
  {
    job_info_io.Message(msg_entry.m_message);
  }
}

void UpdateOutputValueEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue)
{
  auto [decoded, output_entry] = DecodeOutputValueEntry(anyvalue);
  if (decoded)
  {
    job_info_io.PutValue(output_entry.m_value, output_entry.m_description);
  }
}

void UpdateBreakpointInstruction(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue)
{
  auto [valid, instr_idx] = DecodeBreakpointInstructionIndex(anyvalue);
  if (valid)
  {
    job_info_io.BreakpointInstructionUpdated(instr_idx);
  }
}

}  // unnamed namespace
