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
#include <sup/sequencer/execution_status.h>
#include <sup/sequencer/job_states.h>

namespace
{
bool ValidateVariablePayload(const sup::dto::AnyValue& payload);
}

namespace sup
{
namespace auto_server
{

const dto::AnyValue kInstructionAnyValue = {{
  { kExecStatusField, static_cast<dto::uint16>(sequencer::ExecutionStatus::NOT_STARTED)},
  { kBreakpointField, false }
}, kInstructionType };

const dto::AnyValue kInstructionInfoNodeAnyValue = {{
  { kInstructionInfoNodeTypeField, "" },
  { kIndexField, { sup::dto::UnsignedInteger32Type, 0 }},
  { kAttributesField, sup::dto::EmptyStruct() }
}, kInstructionInfoNodeType };

const dto::AnyValue kVariableAnyValue = {{
  { protocol::kEncodingField, protocol::kBase64Encoding},
  { protocol::kValueField, "" }
}, kVariableType };

const dto::AnyValue kVariableInfoAnyValue = {{
  { kVariableInfoTypeField, "" },
  { kIndexField, { sup::dto::UnsignedInteger32Type, 0 }},
  { kAttributesField, sup::dto::EmptyStruct() }
}, kVariableInfoType };

const dto::AnyValue kJobStateAnyValue = {{
  { kJobStateField, static_cast<dto::uint32>(sequencer::JobState::kInitial)}
}, kJobStateType };

std::string GetJobStatePVName(const std::string& prefix)
{
  return prefix + kJobStateId;
}

std::string GetInstructionPVName(const std::string& prefix, dto::uint32 index)
{
  return prefix + kInstructionId + std::to_string(index);
}

std::string GetVariablePVName(const std::string& prefix, dto::uint32 index)
{
  return prefix + kVariableId + std::to_string(index);
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
  if (!payload.HasField(kVariableConnectedField))
  {
    return false;
  }
  if (payload[kVariableConnectedField].GetType() != sup::dto::BooleanType)
  {
    return false;
  }
  return true;
}
}

