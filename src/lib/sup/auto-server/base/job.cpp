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

#include <sup/auto-server/job.h>

#include <sup/auto-server/epics/epics_server_interface.h>

#include <sup/auto-server/server_job_interface.h>

namespace sup
{
namespace auto_server
{

struct Job::JobImpl
{
  JobImpl(const std::string& prefix, sup::sequencer::Procedure& proc);
  ~JobImpl() = default;

  EPICSServerInterface m_epics_server;
  ServerJobInterface m_job_interface;
  sup::sequencer::JobController m_controller;
};

Job::Job(const std::string& prefix, std::unique_ptr<sup::sequencer::Procedure> proc)
  : m_proc{std::move(proc)}
  , m_impl{new JobImpl{prefix, *m_proc}}
{}

Job::~Job() = default;

void Job::SetBreakpoint(const sup::sequencer::Instruction* instruction)
{
  return Controller().SetBreakpoint(instruction);
}

void Job::RemoveBreakpoint(const sup::sequencer::Instruction* instruction)
{
  return Controller().RemoveBreakpoint(instruction);
}

void Job::Start()
{
  return Controller().Start();
}

void Job::Step()
{
  return Controller().Step();
}

void Job::Pause()
{
  return Controller().Pause();
}

void Job::Reset()
{
  return Controller().Reset();
}

void Job::Halt()
{
  return Controller().Halt();
}

sup::sequencer::JobController& Job::Controller()
{
  return m_impl->m_controller;
}

Job::JobImpl::JobImpl(const std::string& prefix, sup::sequencer::Procedure& proc)
  : m_epics_server{}
  , m_job_interface{prefix, proc, m_epics_server}
  , m_controller{proc, m_job_interface}
{}

}  // namespace auto_server

}  // namespace sup
