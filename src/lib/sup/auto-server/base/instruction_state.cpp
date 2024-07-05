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

#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{

InstructionState ToInstructionState(const sup::dto::AnyValue& state_av)
{
  // TODO: validate
  InstructionState result;
  result.m_breakpoint_set = state_av[kBreakpointField].As<sup::dto::boolean>();
  result.m_execution_status =
    static_cast<sequencer::ExecutionStatus>(state_av[kExecStatusField].As<sup::dto::uint16>());
  return result;
}

}  // namespace auto_server

}  // namespace sup
