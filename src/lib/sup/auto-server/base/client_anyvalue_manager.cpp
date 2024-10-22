/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVER
 *
 * Description   : SUP automation server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/auto-server/client_anyvalue_manager.h>

#include <sup/auto-server/output_entry_helper.h>
#include <sup/auto-server/output_entry_types.h>

#include <sup/sequencer/anyvalue_utils.h>

namespace
{
using namespace sup::auto_server;
using sup::sequencer::IJobInfoIO;

void UpdateJobState(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue);
void UpdateInstructionState(IJobInfoIO& job_info_io, sup::dto::uint32 instr_idx,
                            const sup::dto::AnyValue& anyvalue);
void UpdateVariableState(IJobInfoIO& job_info_io, sup::dto::uint32 var_idx,
                        const sup::dto::AnyValue& anyvalue);
void UpdateLogEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue);
void UpdateMessageEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue);
void UpdateOutputValueEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{

ClientAnyValueManager::ClientAnyValueManager(IJobInfoIO& job_info_io)
  : m_job_info_io{job_info_io}
  , m_cb_map{}
{}

ClientAnyValueManager::~ClientAnyValueManager() = default;

bool ClientAnyValueManager::AddAnyValues(const NameAnyValueSet& name_value_set)
{
  sup::dto::uint32 n_instr = 0;
  for (auto& name_value_pair : name_value_set)
  {
    auto& value_name = name_value_pair.first;
    if (m_cb_map.find(value_name) != m_cb_map.end())
    {
      return false;
    }
    auto value_name_info = ParseValueName(value_name);
    if (value_name_info.val_type == ValueNameType::kInstruction)
    {
      ++n_instr;
    }
    auto cb = CreateCallback(value_name_info);
    cb(m_job_info_io, name_value_pair.second);
    m_cb_map[value_name] = cb;
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

sup::dto::AnyValue ClientAnyValueManager::GetUserInput(const std::string& input_server_name,
                                                       const AnyValueInputRequest& request)
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
        return {};
      }
      auto result = m_job_info_io.GetUserValue(value, description);
      return CreateUserValueReply(result, value);
    }
  case InputRequestType::kUserChoice:
    {
      std::vector<std::string> options{};
      sup::dto::AnyValue metadata{};
      if (!ParseUserChoiceRequest(request, options, metadata))
      {
        return false;
      }
      auto choice = m_job_info_io.GetUserChoice(options, metadata);
      return CreateUserChoiceReply(true, choice);
    }
  default:
    break;
  }
  return {};
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

}  // namespace auto_server

}  // namespace sup

namespace
{
void UpdateJobState(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue)
{
  if (!sup::sequencer::utils::ValidateMemberType(anyvalue, kJobStateField,
                                                 sup::dto::UnsignedInteger32Type))
  {
    return;
  }
  auto job_state_int = anyvalue[kJobStateField].As<sup::dto::uint32>();
  auto job_state = static_cast<sup::sequencer::JobState>(job_state_int);
  job_info_io.JobStateUpdated(job_state);
}

void UpdateInstructionState(IJobInfoIO& job_info_io, sup::dto::uint32 instr_idx,
                            const sup::dto::AnyValue& anyvalue)
{
  try
  {
    auto instr_state = sup::sequencer::ToInstructionState(anyvalue);
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
  auto var_state = DecodeVariableState(anyvalue);
  if (sup::dto::IsEmptyValue(var_state.first))
  {
    return;
  }
  job_info_io.VariableUpdated(var_idx, var_state.first, var_state.second);
}

void UpdateLogEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue)
{
  if (!ValidateLogEntryAnyValue(anyvalue))
  {
    return;
  }
  auto log_entry = DecodeLogEntry(anyvalue);
  job_info_io.Log(log_entry.m_severity, log_entry.m_message);
}

void UpdateMessageEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue)
{
  if (!ValidateMessageEntryAnyValue(anyvalue))
  {
    return;
  }
  auto msg_entry = DecodeMessageEntry(anyvalue);
  job_info_io.Message(msg_entry.m_message);
}

void UpdateOutputValueEntry(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue)
{
  if (!ValidateOutputValueEntryAnyValue(anyvalue))
  {
    return;
  }
  auto out_val_entry = DecodeOutputValueEntry(anyvalue);
  job_info_io.PutValue(out_val_entry.m_value, out_val_entry.m_description);
}


}  // unnamed namespace
