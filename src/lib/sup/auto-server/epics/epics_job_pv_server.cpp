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

#include "epics_job_pv_server.h"

#include <sup/auto-server/epics/epics_pv_handler.h>

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/procedure.h>

namespace sup
{
namespace auto_server
{

EPICSJobPVServer::EPICSJobPVServer(const std::string& prefix, const sequencer::Procedure& proc)
  : m_instr_tree_cache{proc.RootInstruction()}
  , m_instr_tree_anyvalue{m_instr_tree_cache.GetInitialInstructionTreeAnyValue()}
  , m_job_state{kJobStateAnyValue}
  , m_pv_handler{new EPICSPVHandler{JobPVInfo{prefix, m_instr_tree_anyvalue, 0}}}
{}

EPICSJobPVServer::~EPICSJobPVServer() = default;

void EPICSJobPVServer::UpdateJobStatePV(sequencer::JobState state)
{
  m_job_state[kJobStateField] = static_cast<sup::dto::uint32>(state);
  m_pv_handler->UpdateJobState(m_job_state);
}

void EPICSJobPVServer::UpdateInstructionStatusPV(const sequencer::Instruction* instruction,
                                                 sequencer::ExecutionStatus status)
{
  auto path = m_instr_tree_cache.FindInstructionPath(instruction);
  auto& instr_node = path.empty() ? m_instr_tree_anyvalue
                                  : m_instr_tree_anyvalue[path];
  instr_node[kExecStatusField] = static_cast<sup::dto::uint16>(status);
  m_pv_handler->UpdateInstructionTree(m_instr_tree_anyvalue);
}

void EPICSJobPVServer::UpdateInstructionBreakpointPV(const sequencer::Instruction* instruction,
                                                     bool breakpoint_set)
{
  auto path = m_instr_tree_cache.FindInstructionPath(instruction);
  auto& instr_node = path.empty() ? m_instr_tree_anyvalue
                                  : m_instr_tree_anyvalue[path];
  instr_node[kBreakpointField] = breakpoint_set;
  m_pv_handler->UpdateInstructionTree(m_instr_tree_anyvalue);
}

}  // namespace auto_server

}  // namespace sup
