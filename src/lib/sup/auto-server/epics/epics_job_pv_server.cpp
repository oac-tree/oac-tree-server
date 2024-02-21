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

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/procedure.h>

namespace sup
{
namespace auto_server
{

EPICSJobPVServer::EPICSJobPVServer(const sequencer::Procedure& proc)
  : m_instr_tree_cache{proc.RootInstruction()}
  , m_instr_tree_anyvalue{m_instr_tree_cache.GetInitialInstructionTreeAnyValue()}
{}

EPICSJobPVServer::~EPICSJobPVServer() = default;

void EPICSJobPVServer::UpdateInstructionStatusPV(const sequencer::Instruction* instruction,
                                                 sequencer::ExecutionStatus status)
{
  auto path = m_instr_tree_cache.FindInstructionPath(instruction);
  auto& instr_node = path.empty() ? m_instr_tree_anyvalue
                                  : m_instr_tree_anyvalue[path];
  instr_node[kExecStatusField] = static_cast<sup::dto::uint16>(status);
  // TODO: Update EPICS server PV
}

void EPICSJobPVServer::UpdateInstructionBreakpointPV(const sequencer::Instruction* instruction,
                                                     sequencer::Breakpoint::Status status)
{
  auto path = m_instr_tree_cache.FindInstructionPath(instruction);
  auto& instr_node = path.empty() ? m_instr_tree_anyvalue
                                  : m_instr_tree_anyvalue[path];
  instr_node[kBreakpointField] = static_cast<sup::dto::uint16>(status);
  // TODO: Update EPICS server PV
}

}  // namespace auto_server

}  // namespace sup
