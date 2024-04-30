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
#include <sup/auto-server/variable_map.h>

#include <memory>

namespace sup
{
namespace sequencer
{
class Procedure;
}  // namespace sequencer

namespace auto_server
{
class EPICSInstructionPVHandler;
class EPICSJobPVHandler;
/**
 * @brief EPICSJobPVServer implements IJobServer and serves its information over EPICS PvAccess
 * server PVs.
 */
class EPICSJobPVServer : public IJobPVServer
{
public:
  EPICSJobPVServer(const std::string& prefix, const sequencer::Procedure& proc);
  ~EPICSJobPVServer();

  void Initialize(const sequencer::Instruction* root) override;

  void UpdateJobStatePV(sequencer::JobState state) override;

  void UpdateInstructionStatusPV(const sequencer::Instruction* instruction,
                                 sequencer::ExecutionStatus status) override;

  void UpdateInstructionBreakpointPV(const sequencer::Instruction* instruction,
                                     bool breakpoint_set) override;

  void UpdateVariable(const std::string& name, const sup::dto::AnyValue& value,
                      bool connected) override;
private:
  const std::string m_prefix;
  InstructionTreeCache m_instr_tree_cache;
  const VariableMap m_variable_map;
  sup::dto::AnyValue m_instr_tree_anyvalue;
  sup::dto::AnyValue m_job_state;
  std::unique_ptr<EPICSJobPVHandler> m_job_pv_handler;
  std::unique_ptr<EPICSInstructionPVHandler> m_instruction_pv_handler;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_JOB_PV_SERVER_H_
