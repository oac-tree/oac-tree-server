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

#include "epics_pv_handler.h"

#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{

EPICSPVHandler::EPICSPVHandler(const std::string& prefix, const sup::dto::AnyValue& instr_tree)
  : m_jobstate_channel{GetJobStatePVName(prefix)}
  , m_instruction_tree_channel{GetInstructionTreePVName(prefix)}
  , m_server{}
{
  m_server.AddVariable(m_jobstate_channel, kJobStateAnyValue);
  m_server.AddVariable(m_instruction_tree_channel, instr_tree);
  m_server.Start();
}

EPICSPVHandler::~EPICSPVHandler() = default;

void EPICSPVHandler::UpdateJobState(const sup::dto::AnyValue& job_state)
{
  m_server.SetValue(m_jobstate_channel, job_state);
}

void EPICSPVHandler::UpdateInstructionTree(const sup::dto::AnyValue& instr_tree)
{
  m_server.SetValue(m_instruction_tree_channel, instr_tree);
}

}  // namespace auto_server

}  // namespace sup
