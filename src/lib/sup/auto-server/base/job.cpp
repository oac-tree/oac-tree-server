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

#include <sup/auto-server/epics/epics_anyvalue_server.h>

#include <sup/auto-server/automation_job_interface.h>

namespace sup
{
namespace auto_server
{

struct Job::JobImpl
{
  JobImpl(const std::string& prefix, std::unique_ptr<sup::sequencer::Procedure> proc);
  ~JobImpl() = default;

  std::unique_ptr<sup::sequencer::Procedure> m_proc;
  EPICSAnyValueServer m_epics_server;
  AutomationJobInterface m_job_interface;
  sup::sequencer::AsyncRunner m_runner;
  JobInfo m_job_info;
  std::vector<const sequencer::Instruction*> m_ordered_instructions;
};

Job::Job(const std::string& prefix, std::unique_ptr<sup::sequencer::Procedure> proc)
  : m_impl{new JobImpl{prefix, std::move(proc)}}
{}

Job::~Job() = default;

Job::Job(Job&& other)
  : m_impl{}
{
  std::swap(m_impl, other.m_impl);
}

JobInfo Job::GetInfo() const
{
  return m_impl->m_job_info;
}

void Job::SetBreakpoint(std::size_t instr_idx)
{
  if (instr_idx >= m_impl->m_ordered_instructions.size())
  {
    return;
  }
  auto instruction = m_impl->m_ordered_instructions[instr_idx];
  return Runner().SetBreakpoint(instruction);
}

void Job::RemoveBreakpoint(std::size_t instr_idx)
{
  if (instr_idx >= m_impl->m_ordered_instructions.size())
  {
    return;
  }
  auto instruction = m_impl->m_ordered_instructions[instr_idx];
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

Job::JobImpl::JobImpl(const std::string& prefix, std::unique_ptr<sup::sequencer::Procedure> proc)
  : m_proc{std::move(proc)}
  , m_epics_server{}
  , m_job_interface{prefix, *m_proc, m_epics_server}
  , m_runner{*m_proc, m_job_interface}
  , m_job_info{prefix, *m_proc}
  , m_ordered_instructions{}
{
  const auto root = m_proc->RootInstruction();
  m_job_interface.InitializeInstructionTree(root);
  m_job_info.SetInstructionTreeInfo(m_job_interface.GetInstructionTreeInfo(root));
  m_ordered_instructions = m_job_interface.GetOrderedInstructions();
}

}  // namespace auto_server

}  // namespace sup
