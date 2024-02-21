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

#ifndef SUP_AUTO_SERVER_EPICS_JOB_PV_SERVER_H_
#define SUP_AUTO_SERVER_EPICS_JOB_PV_SERVER_H_

#include <sup/auto-server/i_job_pv_server.h>
#include <sup/auto-server/instruction_tree_cache.h>

namespace sup
{
namespace sequencer
{
class Procedure;
}  // namespace sequencer

namespace auto_server
{

/**
 * @brief EPICSJobPVServer implements IJobServer and serves its information over EPICS PvAccess
 * server PVs.
 */
class EPICSJobPVServer : public IJobPVServer
{
public:
  EPICSJobPVServer(const sequencer::Procedure& proc);
  ~EPICSJobPVServer();

  void UpdateInstructionStatusPV(const sequencer::Instruction* instruction,
                                 sequencer::ExecutionStatus status) override;

  void UpdateInstructionBreakpointPV(const sequencer::Instruction* instruction,
                                     sequencer::Breakpoint::Status status) override;

private:
  const InstructionTreeCache m_instr_tree_cache;
  sup::dto::AnyValue m_instr_tree_anyvalue;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_JOB_PV_SERVER_H_
