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
  : m_prefix{prefix}
  , m_server{}
{
  m_server.AddVariable(GetJobStatePVName(m_prefix), kJobStateAnyValue);
  m_server.AddVariable(GetInstructionTreePVName(m_prefix), instr_tree);
  m_server.Start();
}

EPICSPVHandler::~EPICSPVHandler() = default;

void EPICSPVHandler::UpdateJobState(const sup::dto::AnyValue& job_state)
{
  m_server.SetValue(GetJobStatePVName(m_prefix), job_state);
}

void EPICSPVHandler::UpdateInstructionTree(const sup::dto::AnyValue& instr_tree)
{
  m_server.SetValue(GetInstructionTreePVName(m_prefix), instr_tree);
}

std::string GetJobStatePVName(const std::string& prefix)
{
  return prefix + kJobStateId;
}

std::string GetInstructionTreePVName(const std::string& prefix)
{
  return prefix + kInstructionTreeId;
}


}  // namespace auto_server

}  // namespace sup
