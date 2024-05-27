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
  sup::sequencer::AsyncRunner m_runner;
};

Job::Job(const std::string& prefix, std::unique_ptr<sup::sequencer::Procedure> proc)
  : m_proc{std::move(proc)}
  , m_impl{new JobImpl{prefix, *m_proc}}
  , m_job_prefix{prefix}
  , m_full_name{sup::sequencer::GetProcedureName(*m_proc)}
  , m_nr_vars{m_proc->VariableNames().size()}
{}

Job::~Job() = default;

std::string Job::GetPrefix() const
{
  return m_job_prefix;
}

std::string Job::GetProcedureName() const
{
  return m_full_name;
}

std::size_t Job::GetNumberOfVariables() const
{
  return m_nr_vars;
}

void Job::SetBreakpoint(const sup::sequencer::Instruction* instruction)
{
  return Runner().SetBreakpoint(instruction);
}

void Job::RemoveBreakpoint(const sup::sequencer::Instruction* instruction)
{
  return Runner().RemoveBreakpoint(instruction);
}

void Job::Start()
{
  return Runner().Start();
}

void Job::Step()
{
  return Runner().Step();
}

void Job::Pause()
{
  return Runner().Pause();
}

void Job::Reset()
{
  return Runner().Reset();
}

void Job::Halt()
{
  return Runner().Halt();
}

sup::sequencer::AsyncRunner& Job::Runner()
{
  return m_impl->m_runner;
}

Job::JobImpl::JobImpl(const std::string& prefix, sup::sequencer::Procedure& proc)
  : m_epics_server{}
  , m_job_interface{prefix, proc, m_epics_server}
  , m_runner{proc, m_job_interface}
{}

}  // namespace auto_server

}  // namespace sup
