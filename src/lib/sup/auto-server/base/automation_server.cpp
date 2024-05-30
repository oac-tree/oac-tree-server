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

JobInfo AutomationServer::GetJobInfo(std::size_t idx) const
{
  if (idx >= m_jobs.size())
  {
    const std::string error = "AutomationServer::GetJobInfo(): index out of bounds; requesting"
      + std::to_string(idx) + " out of " + std::to_string(m_jobs.size()) + " jobs";
    throw InvalidOperationException(error);
  }
  return m_jobs[idx].GetInfo();
}

void AutomationServer::SendJobCommand(std::size_t idx, sup::sequencer::JobCommand command)
{
  using sup::sequencer::JobCommand;
  if (idx >= m_jobs.size())
  {
    const std::string error = "AutomationServer::GetJobInfo(): index out of bounds; requesting"
      + std::to_string(idx) + " out of " + std::to_string(m_jobs.size()) + " jobs";
    throw InvalidOperationException(error);
  }
  switch (command)
  {
  case JobCommand::kStart:
    m_jobs[idx].Start();
    break;
  case JobCommand::kStep:
    m_jobs[idx].Step();
    break;
  case JobCommand::kPause:
    m_jobs[idx].Pause();
    break;
  case JobCommand::kReset:
    m_jobs[idx].Reset();
    break;
  case JobCommand::kHalt:
    m_jobs[idx].Halt();
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
