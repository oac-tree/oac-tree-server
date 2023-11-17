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

#include "job_controller.h"

namespace sup
{
namespace auto_server
{

JobController::JobController(sup::sequencer::Procedure& proc, sup::sequencer::UserInterface& ui)
  : m_loop_future{}
  , m_terminate{false}
{
  (void)proc;
  (void)ui;
  // The ExecutionLoop is called here using std::async and will keep running until destruction of
  // this JobController.
}

// Terminate procedure's execution here
JobController::~JobController() = default;

void JobController::Start()
{
  // push start command to command queue (with lock). Similar for all other commands
  return;
}

void JobController::Pause()
{
  return;
}

void JobController::Resume()
{
  return;
}

void JobController::Step()
{
  return;
}

void JobController::Terminate()
{
  return;
}

void JobController::ExecutionLoop()
{
  while (!m_terminate)
  {
    // If no commands in queue and procedure is not running (continuous execution, not step-by-step):
    //   wait for command stack non-empty
    // Pop command from queue and process it; if appropriate, keep popping and processing
    // (e.g. Step should result in a procedure execution step before processing other commands)
    // If execution step required (determined by current state):
    //   If procedure has no synchronous instructions waiting, i.e. it has ExecutionStatus::RUNNING,
    //   perform a short timeout (see e.g. main.cpp in sequencer-daemon)
    //   m_proc.ExecuteSingle(m_ui)
  }
  // Cleanup: termination should only be used during JobController's destruction so make sure the
  // procedure is halted and finished. All remaining commands in the queue are dumped too.
}

}  // namespace auto_server

}  // namespace sup
