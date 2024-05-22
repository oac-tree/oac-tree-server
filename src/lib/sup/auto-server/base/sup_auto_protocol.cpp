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

namespace sup
{
namespace auto_server
{

const dto::AnyValue kInstructionAnyValue = {{
  { kExecStatusField, static_cast<dto::uint16>(sequencer::ExecutionStatus::NOT_STARTED)},
  { kBreakpointField, false },
  { kChildrenField, dto::EmptyStruct() }
}, kInstructionNodeType };

const dto::AnyValue kVariableAnyValue = {{
  { protocol::kEncodingField, protocol::kBase64Encoding},
  { protocol::kValueField, "" }
}, kVariableType };

const dto::AnyValue kJobStateAnyValue = {{
  { kJobStateField, static_cast<dto::uint32>(sequencer::JobState::kInitial)}
}, kJobStateType };

std::string GetJobStatePVName(const std::string& prefix)
{
  return prefix + kJobStateId;
}

std::string GetInstructionTreePVName(const std::string& prefix)
{
  return prefix + kInstructionTreeId;
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

dto::AnyValue EncodeVariableInfo(const std::string& name, const dto::AnyValue& value,
                                 bool connected)
{
  dto::AnyValue payload = {{
    { kVariableNameField, name },
    { kVariableValueField, value },
    { kVariableConnectedField, connected }
  }};
  auto encoded = protocol::Base64VariableCodec::Encode(payload);
  if (!encoded.first)
  {
    const std::string error = "EncodeVariableInfo(): could not encode the variable's state";
    throw InvalidOperationException(error);
  }
  return encoded.second;
}

}  // namespace auto_server

}  // namespace sup
