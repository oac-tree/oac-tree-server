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
{
  // Procedure MUST already be setup (since it's instruction tree cached should already have been built)
  SetState(JobState::kInitial);
  Launch();
}

// Terminate procedure's execution here
JobController::~JobController()
{
  Terminate();
}

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

void JobController::Halt()
{
  m_runner.Halt();
  m_command_queue.Push(JobCommand::kHalt);
}

void JobController::Terminate()
{
  m_runner.Halt();
  m_command_queue.Push(JobCommand::kTerminate);
}

void JobController::SetState(JobState state)
{
  switch (state)
  {
    case JobState::kInitial:
      m_command_handler = &JobController::HandleInitial;
      break;
    case JobState::kPaused:
      // same handler as initial state:
      m_command_handler = &JobController::HandleInitial;
      break;
    case JobState::kStepping:
      // Nothing to do here. State will switch immediately to paused after step.
      break;
    case JobState::kRunning:
      m_command_handler = &JobController::HandleRunning;
      break;
    case JobState::kSucceeded:
    case JobState::kFailed:
    case JobState::kHalted:
      m_command_handler = &JobController::HandleFinished;
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

JobController::Action JobController::HandleCommand(JobCommand command)
{
  return (this->*m_command_handler)(command);
}

JobController::Action JobController::HandleInitial(JobCommand command)
{
  switch (command)
  {
    case JobCommand::kStart:
      SetState(JobState::kRunning);
      return Action::kRun;
    case JobCommand::kStep:
      SetState(JobState::kStepping);
      return Action::kStep;
    case JobCommand::kPause:
      break;
    case JobCommand::kReset:
      break;
    case JobCommand::kHalt:
      break;
    case JobCommand::kTerminate:
      return Action::kExit;
    default:
      break;
  }
  return Action::kContinue;
}

JobController::Action JobController::HandleRunning(JobCommand command)
{
  switch (command)
  {
    case JobCommand::kStart:
      break;
    case JobCommand::kStep:
    case JobCommand::kPause:
      m_runner.Pause();
      SetState(JobState::kPaused);
      break;
    case JobCommand::kReset:
      break;
    case JobCommand::kHalt:
      SetState(JobState::kHalted);
      break;
    case JobCommand::kTerminate:
      return Action::kExit;
    default:
      break;
  }
  return Action::kContinue;
}

JobController::Action JobController::HandlePaused(JobCommand command)
{
  switch (command)
  {
    case JobCommand::kStart:
      SetState(JobState::kRunning);
      return Action::kRun;
    case JobCommand::kStep:
      SetState(JobState::kStepping);
      return Action::kStep;
    case JobCommand::kPause:
      break;
    case JobCommand::kReset:
      break;
    case JobCommand::kHalt:
      SetState(JobState::kHalted);
      break;
    case JobCommand::kTerminate:
      return Action::kExit;
    default:
      break;
  }
  return Action::kContinue;
}

JobController::Action JobController::HandleFinished(JobCommand command)
{
  switch (command)
  {
    case JobCommand::kStart:
      break;
    case JobCommand::kStep:
      break;
    case JobCommand::kPause:
      break;
    case JobCommand::kReset:
      // TODO: Reset procedure!
      SetState(JobState::kInitial);
      break;
    case JobCommand::kHalt:
      break;
    case JobCommand::kTerminate:
      return Action::kExit;
    default:
      break;
  }
  return Action::kContinue;
}

void JobController::ExecutionLoop()
{
  while (true)
  {
    auto command = m_command_queue.WaitForNextCommand();
    auto action = HandleCommand(command);
    switch (action)
    {
      case Action::kContinue:
        continue;
      case Action::kStep:
        StepProcedure();
        break;
      case Action::kRun:
        RunProcedure();
        break;
      case Action::kExit:
        return;
    }
  }
  // Cleanup: termination should only be used during JobController's destruction so make sure the
  // procedure is halted and finished. All remaining commands in the queue are dumped too.
}

void JobController::RunProcedure()
{
  auto tick_callback = [this](const sup::sequencer::Procedure& proc){
    ProcessCommandsWhenRunning();
    TimeoutWhenRunning timeout{TickTimeoutMs(proc)};
    timeout(proc);
    return;
  };
  m_runner.SetTickCallback(tick_callback);
  m_runner.ExecuteProcedure();
}

void JobController::ProcessCommandsWhenRunning()
{
  JobCommand command = JobCommand::kStart;
  if (m_command_queue.TryPop(command))
  {
    auto action = HandleCommand(command);
    switch (action)
    {
      case Action::kContinue:
        break;
      case Action::kStep:
        break;
      case Action::kRun:
        break;
      case Action::kExit:
        // TODO: change so that the execution loop is terminated.
        return;
    }
  }
}

void JobController::StepProcedure()
{
  m_runner.SetTickCallback(EmptyTickCallback{});
  m_runner.ExecuteSingle();
  SetState(JobState::kPaused);
}

EmptyTickCallback::EmptyTickCallback() = default;

EmptyTickCallback::~EmptyTickCallback() = default;

void EmptyTickCallback::operator()(const sup::sequencer::Procedure&) const
{}

}  // namespace auto_server

}  // namespace sup
