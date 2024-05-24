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

#ifndef SUP_AUTO_SERVER_JOB_H_
#define SUP_AUTO_SERVER_JOB_H_

#include <sup/sequencer/instruction.h>
#include <sup/sequencer/job_controller.h>
#include <sup/sequencer/procedure.h>

#include <memory>
#include <string>

namespace sup
{
namespace auto_server
{
/**
 * @brief Job.
 */
class Job
{
public:
  Job(const std::string& prefix, std::unique_ptr<sup::sequencer::Procedure> proc);
  ~Job();

  /**
   * @brief Forward these methods to JobController
   */
  void SetBreakpoint(const sup::sequencer::Instruction* instruction);
  void RemoveBreakpoint(const sup::sequencer::Instruction* instruction);
  void Start();
  void Step();
  void Pause();
  void Reset();
  void Halt();

private:
  sup::sequencer::JobController& Controller();
  std::unique_ptr<sup::sequencer::Procedure> m_proc;
  struct JobImpl;
  std::unique_ptr<JobImpl> m_impl;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_H_
