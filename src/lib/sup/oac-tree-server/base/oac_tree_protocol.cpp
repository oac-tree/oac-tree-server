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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/oac-tree-server/exceptions.h>

#include <sup/protocol/base64_variable_codec.h>
#include <sup/protocol/function_protocol_extract.h>
#include <sup/oac-tree/anyvalue_utils.h>
#include <sup/oac-tree/constants.h>
#include <sup/oac-tree/execution_status.h>
#include <sup/oac-tree/job_states.h>

#include <limits>
#include <map>

namespace
{
bool ValidateVariableAnyValue(const sup::dto::AnyValue& payload);

/**
 * @brief Validate if the provided AnyValue encodes a list of instruction indices.
 *
 * @param payload AnyValue to validate.
 *
 * @return True if the AnyValue encodes a list of instruction indices.
 */
bool ValidateNextInstructionsAnyValue(const sup::dto::AnyValue& payload);

bool EndsWith(const std::string& str, const std::string& sub_str);

bool ParseIndex(const std::string& idx_str, sup::dto::uint32& idx);
}

namespace sup
{
namespace oac_tree_server
{

const sup::dto::AnyValue kVariableAnyValue = {{
  { kVariableValueField, {} },
  { kVariableConnectedField, false }
}, kVariableType };

const sup::dto::AnyValue kInputRequestAnyValue = {{
  { kInputRequestIndexField, { sup::dto::UnsignedInteger64Type, 0 }},
  { kInputRequestTypeField, { sup::dto::UnsignedInteger32Type, 0 }},
  { kInputRequestMetadataField, {} },
  { kInputRequestInputTypeField, { sup::dto::StringType, "" }}
}, kInputRequestType };

const sup::dto::AnyValue kLogEntryAnyValue = {{
  { sup::oac_tree::Constants::kIndexField, { sup::dto::UnsignedInteger64Type, 0 } },
  { kSeverityField, { sup::dto::SignedInteger32Type, 0 } },
  { kMessageField, "" }
}, kLogEntryType };

const sup::dto::AnyValue kMessageEntryAnyValue = {{
  { sup::oac_tree::Constants::kIndexField, { sup::dto::UnsignedInteger64Type, 0 } },
  { kMessageField, "" }
}, kMessageEntryType };

const sup::dto::AnyValue kOutputValueEntryAnyValue = {{
  { sup::oac_tree::Constants::kIndexField, { sup::dto::UnsignedInteger64Type, 0 } },
  { kDescriptionField, "" },
  { kValueField, {} }
}, kOutputValueEntryType };

const sup::dto::AnyValue kNextInstructionsAnyValue =
  sup::dto::AnyValue{0, sup::dto::UnsignedInteger32Type, kNextInstructionsType};

const sup::dto::AnyValue kJobStateAnyValue = {{
  { kJobStateField, static_cast<sup::dto::uint32>(oac_tree::JobState::kInitial)}
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

std::string CreateJobPrefix(const std::string& server_prefix, sup::dto::uint32 idx)
{
  return server_prefix + ":PROC-" + std::to_string(idx) + ":";
}

std::string GetInstructionPVName(const std::string& prefix, sup::dto::uint32 index)
{
  return prefix + kInstructionId + std::to_string(index);
}

std::string GetVariablePVName(const std::string& prefix, sup::dto::uint32 index)
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

std::string GetNextInstructionsName(const std::string& prefix)
{
  return prefix + kNextInstructionsId;
}

std::string GetJobStatePVName(const std::string& prefix)
{
  return prefix + kJobStateId;
}

sup::dto::AnyValue GetJobStateValue(oac_tree::JobState state)
{
  auto result = kJobStateAnyValue;
  result[kJobStateField] = static_cast<sup::dto::uint32>(state);
  return result;
}

sup::dto::AnyValue EncodeVariableState(const sup::dto::AnyValue& value, bool connected)
{
  sup::dto::AnyValue var_state = {{
    { kVariableValueField, value },
    { kVariableConnectedField, connected }
  }};
  return var_state;
}

std::pair<sup::dto::AnyValue, bool> DecodeVariableState(const sup::dto::AnyValue& encoded)
{
  if (ValidateVariableAnyValue(encoded))
  {
    return { encoded[kVariableValueField], encoded[kVariableConnectedField].As<sup::dto::boolean>() };
  }
  return { {}, false };
}

sup::dto::AnyValue EncodeNextInstructionIndices(const std::vector<sup::dto::uint32>& next_indices)
{
  sup::dto::AnyValue encoded{next_indices.size(), sup::dto::UnsignedInteger32Type};
  for (std::size_t i=0; i<next_indices.size(); ++i)
  {
    encoded[i] = next_indices[i];
  }
  return encoded;
}

std::pair<bool, std::vector<sup::dto::uint32>> DecodeNextInstructionIndices(
  const sup::dto::AnyValue& encoded)
{
  if (ValidateNextInstructionsAnyValue(encoded))
  {
    std::vector<sup::dto::uint32> indices;
    for (size_t i = 0; i < encoded.NumberOfElements(); ++i)
    {
      indices.push_back(encoded[i].As<sup::dto::uint32>());
    }
    return { true, indices };
  }
  return { false, {} };
}

ValueNameInfo ParseValueName(const std::string& val_name)
{
  static const std::vector<std::pair<std::string, ValueNameType>> postfixes = {
    { kJobStateId, ValueNameType::kJobStatus },
    { kLogEntryId, ValueNameType::kLogEntry },
    { kMessageEntryId, ValueNameType::kMessageEntry },
    { kOutputValueEntryId, ValueNameType::kOutputValueEntry },
    { kNextInstructionsId, ValueNameType::kNextInstructions }
  };
  ValueNameInfo unknown{ ValueNameType::kUnknown, 0 };
  for (const auto& [postfix, postfix_type] : postfixes)
  {
    if (EndsWith(val_name, postfix))
    {
      if (val_name.size() == postfix.size())
      {
        return unknown;
      }
      return { postfix_type, 0 };
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

sup::protocol::ProtocolResult ExtractJobIndex(
  const sup::dto::AnyValue& input, sup::dto::uint32 n_jobs, sup::dto::uint32& idx)
{
  sup::dto::AnyValue idx_av{};
  if (!sup::protocol::FunctionProtocolExtract(idx_av, input, kJobIndexFieldName))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  if (!idx_av.As(idx))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  if (idx >= n_jobs)
  {
    return UnknownJob;
  }
  return sup::protocol::Success;
}

sup::protocol::ProtocolResult ExtractInstructionIndex(
  const sup::dto::AnyValue& input, sup::dto::uint32 n_instr, sup::dto::uint32& idx)
{
  sup::dto::AnyValue idx_av{};
  if (!sup::protocol::FunctionProtocolExtract(idx_av, input, kInstructionIndexFieldName))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  if (!idx_av.As(idx))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  if (idx >= n_instr)
  {
    return UnknownInstruction;
  }
  return sup::protocol::Success;
}

sup::dto::AnyValue Base64EncodeAnyValue(const sup::dto::AnyValue& value)
{
  auto [encoded, base64value] = sup::protocol::Base64VariableCodec::Encode(value);
  if (!encoded)
  {
    const std::string error = "Base64EncodeAnyValue(): could not base64 encode an AnyValue";
    throw InvalidOperationException(error);
  }
  return base64value;
}

std::pair<bool, sup::dto::AnyValue> Base64DecodeAnyValue(const sup::dto::AnyValue& value)
{
  auto [decoded, anyvalue] = sup::protocol::Base64VariableCodec::Decode(value);
  if (decoded)
  {
    return { true, anyvalue };
  }
  return { false, {} };
}

}  // namespace oac_tree_server

}  // namespace sup

namespace
{
using namespace sup::oac_tree_server;
bool ValidateVariableAnyValue(const sup::dto::AnyValue& var_value)
{
  if (!var_value.HasField(kVariableValueField))
  {
    return false;
  }
  if (!sup::oac_tree::utils::ValidateMemberType(var_value, kVariableConnectedField,
                                                 sup::dto::BooleanType))
  {
    return false;
  }
  return true;
}

bool ValidateNextInstructionsAnyValue(const sup::dto::AnyValue& next_instr_av)
{
  if (!sup::dto::IsArrayValue(next_instr_av))
  {
    return false;
  }
  if (next_instr_av.GetType().ElementType() != sup::dto::UnsignedInteger32Type)
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

