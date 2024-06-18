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

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/automation_server.h>

namespace sup
{
namespace auto_server
{

AutomationServer::AutomationServer(const std::string& server_prefix, ProcedureList& proc_list)
  : m_server_prefix{server_prefix}
  , m_jobs{}
{
  for (std::size_t idx = 0; idx < proc_list.size(); ++idx)
  {
    m_jobs.emplace_back(CreateJobPrefix(m_server_prefix, idx), std::move(proc_list[idx]));
  }
}

AutomationServer::~AutomationServer() = default;

std::string AutomationServer::GetServerPrefix() const
{
  return m_server_prefix;
}

std::size_t AutomationServer::GetNumberOfJobs() const
{
  return m_jobs.size();
}

const OldJobInfo& AutomationServer::GetJobInfo(std::size_t job_idx) const
{
  if (job_idx >= m_jobs.size())
  {
    const std::string error = "AutomationServer::GetJobInfo(): index out of bounds; requesting"
      + std::to_string(job_idx) + " out of " + std::to_string(m_jobs.size()) + " jobs";
    throw InvalidOperationException(error);
  }
  return m_jobs[job_idx].GetInfo();
}

void AutomationServer::EditBreakpoint(std::size_t job_idx, std::size_t instr_idx,
                                      bool breakpoint_active)
{
  if (job_idx >= m_jobs.size())
  {
    const std::string error = "AutomationServer::EditBreakpoint(): index out of bounds; requesting"
      + std::to_string(job_idx) + " out of " + std::to_string(m_jobs.size()) + " jobs";
    throw InvalidOperationException(error);
  }
  if (breakpoint_active)
  {
    m_jobs[job_idx].SetBreakpoint(instr_idx);
  }
  else
  {
    m_jobs[job_idx].RemoveBreakpoint(instr_idx);
  }
  return;
}

void AutomationServer::SendJobCommand(std::size_t job_idx, sup::sequencer::JobCommand command)
{
  using sup::sequencer::JobCommand;
  if (job_idx >= m_jobs.size())
  {
    const std::string error = "AutomationServer::GetJobInfo(): index out of bounds; requesting"
      + std::to_string(job_idx) + " out of " + std::to_string(m_jobs.size()) + " jobs";
    throw InvalidOperationException(error);
  }
  switch (command)
  {
  case JobCommand::kStart:
    m_jobs[job_idx].Start();
    break;
  case JobCommand::kStep:
    m_jobs[job_idx].Step();
    break;
  case JobCommand::kPause:
    m_jobs[job_idx].Pause();
    break;
  case JobCommand::kReset:
    m_jobs[job_idx].Reset();
    break;
  case JobCommand::kHalt:
    m_jobs[job_idx].Halt();
    break;
  default:
    {
      const std::string error = "AutomationServer::GetJobInfo(): unknown command enumerator ["
        + std::to_string(static_cast<int>(command)) + "]";
      throw InvalidOperationException(error);
    }
  }
}

std::string CreateJobPrefix(const std::string& server_prefix, std::size_t idx)
{
  return server_prefix + ":PROC-" + std::to_string(idx) + ":";
}

}  // namespace auto_server

}  // namespace sup
