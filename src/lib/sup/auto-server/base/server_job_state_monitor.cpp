/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVERvirtual
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

#include <sup/auto-server/server_job_state_monitor.h>

namespace sup
{
namespace auto_server
{

ServerJobStateMonitor::ServerJobStateMonitor(IJobPVServer& pv_server)
  : m_pv_server{pv_server}
{}

ServerJobStateMonitor::~ServerJobStateMonitor() = default;

void ServerJobStateMonitor::OnStateChange(sequencer::JobState state) noexcept
{
  m_pv_server.UpdateJobStatePV(state);
}

void ServerJobStateMonitor::OnBreakpointChange(const sequencer::Instruction* instruction,
                          bool breakpoint_set) noexcept
{
  m_pv_server.UpdateInstructionBreakpointPV(instruction, breakpoint_set);
}

}  // namespace auto_server

}  // namespace sup
