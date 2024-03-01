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

const sup::dto::AnyValue kInstructionAnyValue = {{
  { kExecStatusField, static_cast<sup::dto::uint16>(sup::sequencer::ExecutionStatus::NOT_STARTED)},
  { kBreakpointField, false },
  { kChildrenField, sup::dto::EmptyStruct() }
}, kInstructionNodeType };

const sup::dto::AnyValue kVariableAnyValue = {{
  { sup::protocol::kEncodingField, sup::protocol::kBase64Encoding},
  { sup::protocol::kValueField, "" }
}, kVariableType };

const sup::dto::AnyValue kJobStateAnyValue = {{
  { kJobStateField, static_cast<sup::dto::uint32>(sup::sequencer::JobState::kInitial)}
}, kJobStateType };

std::string GetJobStatePVName(const std::string& prefix)
{
  return prefix + kJobStateId;
}

std::string GetInstructionTreePVName(const std::string& prefix)
{
  return prefix + kInstructionTreeId;
}

std::string GetVariablePVName(const std::string& prefix, sup::dto::uint32 index)
{
  return prefix + kVariableId + std::to_string(index);
}

sup::dto::AnyValue EncodeVariableInfo(const sup::dto::AnyValue& value, bool connected)
{
  sup::dto::AnyValue payload = {{
    { kVariableValueField, value },
    { kVariableConnectedField, connected }
  }};
  auto encoded = sup::protocol::Base64VariableCodec::Encode(payload);
  if (!encoded.first)
  {
    const std::string error = "EncodeVariableInfo(): could not encode the variable's state";
    throw InvalidOperationException(error);
  }
  return encoded.second;
}

}  // namespace auto_server

}  // namespace sup
