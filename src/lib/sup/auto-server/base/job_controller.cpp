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

#include "job_controller.h"

#include <sup/sequencer/exceptions.h>
#include <sup/sequencer/log_severity.h>

namespace sup
{
namespace auto_server
{
using namespace sup::sequencer;

JobController::JobController(Procedure& proc, UserInterface& ui)
  : m_proc{proc}
  , m_ui{ui}
  , m_runner{m_ui}
  , m_loop_future{}
  , m_terminate{false}
{
  // Procedure MUST already be setup (since it's instruction tree cached should already have been built)
  // Setup the procedure and send high severity log to ui if this fails
  Setup();
  m_runner.SetProcedure(std::addressof(m_proc));
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
  // TODO
  return;
}

void JobController::Resume()
{
  // TODO
  return;
}

void JobController::Step()
{
  // TODO
  return;
}

void JobController::Terminate()
{
  return;
}

bool JobController::Setup()
{
  try
  {
    m_proc.Setup();
  }
  catch(const MessageException& exception)
  {
    std::string message = "JobController::Setup(): exception thrown in Procedure::Setup(): " +
                          std::string(exception.what());
    m_ui.Log(log::SUP_SEQ_LOG_CRIT, message);
    return false;
  }
  return true;
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
