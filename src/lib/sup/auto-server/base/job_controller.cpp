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
  , m_command_queue{}
  , m_command_handler{}
  , m_loop_future{}
  , m_terminate{false}
{
  // Procedure MUST already be setup (since it's instruction tree cached should already have been built)
  SetState(JobState::kInitial);
  Launch();
}

// Terminate procedure's execution here
JobController::~JobController() = default;

void JobController::Start()
{
  m_command_queue.Push(JobCommand::kStart);
}

void JobController::Step()
{
  m_command_queue.Push(JobCommand::kStep);
}

void JobController::Pause()
{
  m_command_queue.Push(JobCommand::kPause);
}

void JobController::Reset()
{
  m_command_queue.Push(JobCommand::kReset);
}

void JobController::SetState(JobState state)
{
  switch (state)
  {
    case JobState::kInitial:
      m_command_handler = &JobController::HandleInitial;
      break;
    case JobState::kPaused:
    case JobState::kFinished:
    case JobState::kStepping:
    case JobState::kRunning:
      break;
    default:
      break;
  }
}

void JobController::Launch()
{
  m_runner.SetProcedure(std::addressof(m_proc));
  m_loop_future = std::async(std::launch::async, &JobController::ExecutionLoop, this);
}

bool JobController::HandleInitial(JobCommand command)
{
  (void)command;
  return false;
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
