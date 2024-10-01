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

#include <sup/auto-server/instruction_state.h>

#include <sup/auto-server/anyvalue_utils.h>
#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{

bool operator==(const InstructionState& left, const InstructionState& right)
{
  if (left.m_breakpoint_set != right.m_breakpoint_set)
  {
    return false;
  }
  if (left.m_execution_status != right.m_execution_status)
  {
    return false;
  }
  return true;
}

bool operator!=(const InstructionState& left, const InstructionState& right)
{
  return !(left == right);
}


sup::dto::AnyValue ToAnyValue(const InstructionState& state)
{
  auto result = kInstructionAnyValue;
  result[kExecStatusField] = static_cast<sup::dto::uint16>(state.m_execution_status);
  result[kBreakpointField] = state.m_breakpoint_set;
  return result;
}

InstructionState ToInstructionState(const sup::dto::AnyValue& state_av)
{
  if (!utils::ValidateMemberType(state_av, kExecStatusField, sup::dto::UnsignedInteger16Type))
  {
    const std::string error = "ToInstructionState(): could not parse provided AnyValue to an "
      "InstructionState object";
    throw InvalidOperationException(error);
  }
  if (!utils::ValidateMemberType(state_av, kBreakpointField, sup::dto::BooleanType))
  {
    const std::string error = "ToInstructionState(): could not parse provided AnyValue to an "
      "InstructionState object";
    throw InvalidOperationException(error);
  }
  InstructionState result;
  result.m_breakpoint_set = state_av[kBreakpointField].As<sup::dto::boolean>();
  result.m_execution_status =
    static_cast<sequencer::ExecutionStatus>(state_av[kExecStatusField].As<sup::dto::uint16>());
  return result;
}

}  // namespace auto_server

}  // namespace sup
