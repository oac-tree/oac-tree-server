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

#ifndef SUP_AUTO_SERVER_JOB_CONTROLLER_H_
#define SUP_AUTO_SERVER_JOB_CONTROLLER_H_

#include "job_command_queue.h"
#include "job_states.h"

#include <sup/sequencer/procedure.h>
#include <sup/sequencer/runner.h>
#include <sup/sequencer/user_interface.h>

#include <future>

namespace sup
{
namespace auto_server
{

/**
 * @brief Asynchronous wrapper around sup::sequencer::Runner.
 *
 * The JobController's responsibility is to asynchronously handle control commands related to a
 * procedure's execution. The main difference with the existing Runner in the Sequencer, which is
 * encapsulated here, is that starting or resuming execution will spawn a thread to handle the
 * procedure's execution.
*/
class JobController
{
public:
  explicit JobController(sup::sequencer::Procedure& proc, sup::sequencer::UserInterface& ui);

  ~JobController();

  /**
   * @brief Run the procedure in a continuous manner if allowed.
   */
  void Start();

  /**
   * @brief Execute a single step of the procedure if allowed.
   */
  void Step();

  /**
   * @brief Interrupt the procedure's execution.
   */
  void Pause();

  /**
   * @brief Reset the procedure to its initial state.
   */
  void Reset();

private:
  sup::sequencer::Procedure& m_proc;
  sup::sequencer::UserInterface& m_ui;
  sup::sequencer::Runner m_runner;

  JobCommandQueue m_command_queue;

  using CommandHandlerFunction = bool (JobController::*)(JobCommand);
  CommandHandlerFunction m_command_handler;
  /**
   * @brief Track the JobController's execution loop.
   */
  std::future<void> m_loop_future;

  std::atomic<bool> m_terminate;

  void SetState(JobState state);

  void Launch();

  bool HandleInitial(JobCommand command);

  void ExecutionLoop();
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_CONTROLLER_H_
