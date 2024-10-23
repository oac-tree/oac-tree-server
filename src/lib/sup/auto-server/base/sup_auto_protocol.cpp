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

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/auto-server/exceptions.h>

#include <sup/protocol/base64_variable_codec.h>
#include <sup/sequencer/anyvalue_utils.h>
#include <sup/sequencer/constants.h>
#include <sup/sequencer/execution_status.h>
#include <sup/sequencer/job_states.h>

#include <limits>
#include <map>

namespace
{
bool ValidateVariablePayload(const sup::dto::AnyValue& payload);

bool EndsWith(const std::string& str, const std::string& sub_str);

bool ParseIndex(const std::string& idx_str, sup::dto::uint32& idx);
}

namespace sup
{
namespace auto_server
{

const dto::AnyValue kVariableAnyValue = {{
  { protocol::kEncodingField, protocol::kBase64Encoding},
  { protocol::kValueField, "" }
}, kVariableType };

const dto::AnyValue kInputRequestAnyValue = {{
  { protocol::kEncodingField, protocol::kBase64Encoding},
  { protocol::kValueField, "" }
}, kInputRequestType };

const dto::AnyValue kLogEntryAnyValue = {{
  { sup::sequencer::Constants::kIndexField, { sup::dto::UnsignedInteger64Type, 0 } },
  { kSeverityField, { sup::dto::SignedInteger32Type, 0 } },
  { kMessageField, "" }
}, kLogEntryType };

const dto::AnyValue kMessageEntryAnyValue = {{
  { sup::sequencer::Constants::kIndexField, { sup::dto::UnsignedInteger64Type, 0 } },
  { kMessageField, "" }
}, kMessageEntryType };

const dto::AnyValue kOutputValueEntryAnyValue = {{
  { sup::sequencer::Constants::kIndexField, { sup::dto::UnsignedInteger64Type, 0 } },
  { kDescriptionField, "" },
  { kValueField, {} }
}, kOutputValueEntryType };

const dto::AnyValue kJobStateAnyValue = {{
  { kJobStateField, static_cast<dto::uint32>(sequencer::JobState::kInitial)}
}, kJobStateType };

namespace status
{
enum AutomationServerStatus
{
  kNotSupported = sup::protocol::SPECIFIC_APPLICATION_ERROR_START,
  kUnknownJob,
  kUnknownInstruction,
  kUnknownJobCommand,
  kClientReplyRefused
};
}  // namespace status

const sup::protocol::ProtocolResult NotSupported{status::kNotSupported};
const sup::protocol::ProtocolResult UnknownJob{status::kUnknownJob};
const sup::protocol::ProtocolResult UnknownInstruction{status::kUnknownInstruction};
const sup::protocol::ProtocolResult UnknownJobCommand{status::kUnknownJobCommand};
const sup::protocol::ProtocolResult ClientReplyRefused{status::kClientReplyRefused};

std::string GetControlServerName(const std::string& server_name)
{
  return server_name + kControlServerId;
}

std::string AutomationServerResultToString(const sup::protocol::ProtocolResult& result)
{
  static std::map<int, std::string> result_map = {
      {status::kNotSupported, "NotSupported"},
      {status::kUnknownJob, "UnknownJob"},
      {status::kUnknownInstruction, "UnknownInstruction"},
      {status::kUnknownJobCommand, "UnknownJobCommand"},
      {status::kClientReplyRefused, "ClientReplyRefused"}};
  if (result.GetValue() < sup::protocol::GENERIC_APPLICATION_ERROR_START)
  {
    return sup::protocol::ProtocolResultToString(result);
  }
  auto it = result_map.find(result.GetValue());
  if (it != result_map.end())
  {
    return it->second;
  }
  return "Unknown ProtocolResult for SUP automation interface: " +
         std::to_string(result.GetValue());
}

std::string CreateJobPrefix(const std::string& server_prefix, std::size_t idx)
{
  return server_prefix + ":PROC-" + std::to_string(idx) + ":";
}

std::string GetInstructionPVName(const std::string& prefix, dto::uint32 index)
{
  return prefix + kInstructionId + std::to_string(index);
}

std::string GetVariablePVName(const std::string& prefix, dto::uint32 index)
{
  return prefix + kVariableId + std::to_string(index);
}

std::string GetInputServerName(const std::string& prefix)
{
  return prefix + kInputServerName;
}

std::string GetInputRequestPVName(const std::string& server_name)
{
  return server_name + kInputRequestName;
}

std::string GetLogEntryName(const std::string& prefix)
{
  return prefix + kLogEntryId;
}

std::string GetMessageEntryName(const std::string& prefix)
{
  return prefix + kMessageEntryId;
}

std::string GetOutputValueEntryName(const std::string& prefix)
{
  return prefix + kOutputValueEntryId;
}

std::string GetJobStatePVName(const std::string& prefix)
{
  return prefix + kJobStateId;
}

dto::AnyValue GetJobStateValue(sequencer::JobState state)
{
  auto result = kJobStateAnyValue;
  result[kJobStateField] = static_cast<dto::uint32>(state);
  return result;
}

dto::AnyValue EncodeVariableState(const dto::AnyValue& value, bool connected)
{
  dto::AnyValue payload = {{
    { kVariableValueField, value },
    { kVariableConnectedField, connected }
  }};
  auto encoded = protocol::Base64VariableCodec::Encode(payload);
  if (!encoded.first)
  {
    const std::string error = "EncodeVariableState(): could not encode the variable's state";
    throw InvalidOperationException(error);
  }
  return encoded.second;
}

std::pair<sup::dto::AnyValue, bool> DecodeVariableState(const dto::AnyValue& encoded)
{
  const std::pair<sup::dto::AnyValue, bool> failure{ {}, false };
  auto decoded = protocol::Base64VariableCodec::Decode(encoded);
  if (!decoded.first)
  {
    return failure;
  }
  auto& payload = decoded.second;
  if (!ValidateVariablePayload(payload))
  {
    return failure;
  }
  return { payload[kVariableValueField], payload[kVariableConnectedField].As<sup::dto::boolean>() };
}

ValueNameInfo ParseValueName(const std::string& val_name)
{
  static const std::vector<std::pair<std::string, ValueNameType>> postfixes = {
    { kJobStateId, ValueNameType::kJobStatus },
    { kLogEntryId, ValueNameType::kLogEntry },
    { kMessageEntryId, ValueNameType::kMessageEntry },
    { kOutputValueEntryId, ValueNameType::kOutputValueEntry }
  };
  ValueNameInfo unknown{ ValueNameType::kUnknown, 0 };
  for (const auto& postfix_type : postfixes)
  {
    if (EndsWith(val_name, postfix_type.first))
    {
      if (val_name.size() == postfix_type.first.size())
      {
        return unknown;
      }
      return { postfix_type.second, 0 };
    }
  }
  auto pos = val_name.find_last_of("-");
  if (pos == std::string::npos)
  {
    return unknown;
  }
  sup::dto::uint32 idx;
  auto idx_str = val_name.substr(pos + 1);
  if (!ParseIndex(idx_str, idx))
  {
    return unknown;
  }
  auto remainder = val_name.substr(0, pos+1);
  if (EndsWith(remainder, kInstructionId) && remainder.size() != kInstructionId.size())
  {
    return { ValueNameType::kInstruction, idx };
  }
  if (EndsWith(remainder, kVariableId) && remainder.size() != kVariableId.size())
  {
    return { ValueNameType::kVariable, idx };
  }
  return unknown;
}

}  // namespace auto_server

}  // namespace sup

namespace
{
using namespace sup::auto_server;
bool ValidateVariablePayload(const sup::dto::AnyValue& payload)
{
  if (!payload.HasField(kVariableValueField))
  {
    return false;
  }
  if (!sup::sequencer::utils::ValidateMemberType(payload, kVariableConnectedField,
                                                 sup::dto::BooleanType))
  {
    return false;
  }
  return true;
}

bool EndsWith(const std::string& str, const std::string& sub_str)
{
  auto pos = str.rfind(sub_str);
  return pos != std::string::npos && pos + sub_str.size() == str.size();
}

bool ParseIndex(const std::string& idx_str, sup::dto::uint32& idx)
{
  unsigned long result;
  try
  {
    result = std::stoul(idx_str);
  }
  catch(const std::exception& e)
  {
    return false;
  }
  if (std::to_string(result) != idx_str)
  {
    return false;
  }
  if (result > std::numeric_limits<sup::dto::uint32>::max())
  {
    return false;
  }
  idx = static_cast<sup::dto::uint32>(result);
  return true;
}

}

