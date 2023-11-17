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
 * Copyright (c) : 2010-2023 ITER Organization,
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

#include <sup/sequencer/procedure.h>
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

  void Start();
  void Pause();
  void Resume();
  void Step();
  void Terminate();

private:
  /**
   * @brief Track the JobController's execution loop.
   */
  std::future<void> m_loop_future;

  std::atomic<bool> m_terminate;

  void ExecutionLoop();
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_CONTROLLER_H_
