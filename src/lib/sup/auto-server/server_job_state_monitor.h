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

#ifndef SUP_AUTO_SERVER_SERVER_JOB_STATE_MONITOR_H_
#define SUP_AUTO_SERVER_SERVER_JOB_STATE_MONITOR_H_

#include <sup/auto-server/i_job_pv_server.h>

#include <sup/sequencer/job_state_monitor.h>

namespace sup
{
namespace auto_server
{

class ServerJobStateMonitor : public sequencer::JobStateMonitor
{
public:
  ServerJobStateMonitor(IJobPVServer& pv_server);
  ~ServerJobStateMonitor();

  void InitializeInstructionTree(const sequencer::Instruction* root) override;

  void OnStateChange(sequencer::JobState state) noexcept override;

  void OnBreakpointChange(const sequencer::Instruction* instruction,
                          bool breakpoint_set) noexcept override;

  void OnProcedureTick(const sequencer::Procedure& proc) noexcept override;

private:
  IJobPVServer& m_pv_server;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SERVER_JOB_STATE_MONITOR_H_
