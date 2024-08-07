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

#include <sup/auto-server/server_job.h>

#include <sup/auto-server/automation_job_interface.h>
#include <sup/auto-server/instruction_map.h>
#include <sup/auto-server/job_utils.h>

#include <sup/sequencer/workspace.h>

namespace sup
{
namespace auto_server
{

struct ServerJob::JobImpl
{
  JobImpl(const std::string& prefix, std::unique_ptr<sup::sequencer::Procedure> proc,
          IJobInfoIO& job_info_io);
  ~JobImpl() = default;

  std::unique_ptr<sup::sequencer::Procedure> m_proc;
  AutomationJobInterface m_job_interface;
  sup::sequencer::AsyncRunner m_runner;
  std::unique_ptr<JobInfo> m_job_info;
  std::vector<const sequencer::Instruction*> m_ordered_instructions;
};

ServerJob::ServerJob(const std::string& prefix, std::unique_ptr<sup::sequencer::Procedure> proc,
         IJobInfoIO& job_info_io)
  : m_impl{new JobImpl{prefix, std::move(proc), job_info_io}}
{}

ServerJob::~ServerJob() = default;

ServerJob::ServerJob(ServerJob&& other)
  : m_impl{}
{
  std::swap(m_impl, other.m_impl);
}

const JobInfo& ServerJob::GetInfo() const
{
  return *m_impl->m_job_info;
}

void ServerJob::SetBreakpoint(std::size_t instr_idx)
{
  if (instr_idx >= m_impl->m_ordered_instructions.size())
  {
    return;
  }
  auto instruction = m_impl->m_ordered_instructions[instr_idx];
  return Runner().SetBreakpoint(instruction);
}

void ServerJob::RemoveBreakpoint(std::size_t instr_idx)
{
  if (instr_idx >= m_impl->m_ordered_instructions.size())
  {
    return;
  }
  auto instruction = m_impl->m_ordered_instructions[instr_idx];
  return Runner().RemoveBreakpoint(instruction);
}

void ServerJob::Start()
{
  return Runner().Start();
}

void ServerJob::Step()
{
  return Runner().Step();
}

void ServerJob::Pause()
{
  return Runner().Pause();
}

void ServerJob::Reset()
{
  return Runner().Reset();
}

void ServerJob::Halt()
{
  return Runner().Halt();
}

sup::sequencer::AsyncRunner& ServerJob::Runner()
{
  return m_impl->m_runner;
}

ServerJob::JobImpl::JobImpl(const std::string& prefix, std::unique_ptr<sup::sequencer::Procedure> proc,
                      IJobInfoIO& job_info_io)
  : m_proc{std::move(proc)}
  , m_job_interface{prefix, *m_proc, job_info_io}
  , m_runner{*m_proc, m_job_interface}
  , m_job_info{}
  , m_ordered_instructions{}
{
  m_job_interface.InitializeInstructionTree(m_proc->RootInstruction());
  m_ordered_instructions = m_job_interface.GetOrderedInstructions();
  m_job_info.reset(new JobInfo{utils::CreateJobInfo(prefix, *m_proc,
                                                    m_job_interface.GetInstructionMap())});
}

}  // namespace auto_server

}  // namespace sup
