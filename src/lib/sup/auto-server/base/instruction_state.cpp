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

InstructionState ToInstructionState(const sup::dto::AnyValue& state_av)
{
  if (!utils::ValidateMemberType(state_av, kBreakpointField, sup::dto::BooleanType))
  {
    const std::string error = "ToInstructionState(): could not parse provided AnyValue to an "
      "InstructionState object";
    throw InvalidOperationException(error);
  }
  if (!utils::ValidateMemberType(state_av, kExecStatusField, sup::dto::UnsignedInteger16Type))
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
