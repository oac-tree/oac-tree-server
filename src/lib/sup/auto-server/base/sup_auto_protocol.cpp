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

const sup::dto::AnyValue kJobStateAnyValue = {{
  { kJobStateField, static_cast<sup::dto::uint32>(sup::sequencer::JobState::kInitial)}
}, kJobStateType };

}  // namespace auto_server

}  // namespace sup
