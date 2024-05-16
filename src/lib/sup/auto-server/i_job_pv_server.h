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

#ifndef SUP_AUTO_SERVER_I_JOB_PV_SERVER_H_
#define SUP_AUTO_SERVER_I_JOB_PV_SERVER_H_

#include <sup/sequencer/instruction.h>
#include <sup/sequencer/job_states.h>

namespace sup
{
namespace auto_server
{

/**
 * @brief IJobPVServer defines an interface for classes that handle all job-related updates, such
 * as job status, its instruction statuses, workspace variable updates, etc.
 */
class IJobPVServer
{
public:
  virtual ~IJobPVServer();

  virtual void Initialize(const sequencer::Instruction* root) = 0;

  /**
   * @brief Handle JobStateMonitor calls
   */
  virtual void UpdateJobStatePV(sequencer::JobState state) = 0;
  virtual void UpdateInstructionBreakpointPV(const sequencer::Instruction* instruction,
                                             bool breakpoint_set) = 0;

  /**
   * @brief Handle UserInterface calls
   */
  virtual void UpdateInstructionStatusPV(const sequencer::Instruction* instruction,
                                         sequencer::ExecutionStatus status) = 0;
  virtual void UpdateVariable(const std::string& name, const sup::dto::AnyValue& value,
                              bool connected) = 0;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_JOB_PV_SERVER_H_
