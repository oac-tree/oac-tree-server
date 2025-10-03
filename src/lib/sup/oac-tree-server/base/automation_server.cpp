/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : oac-tree server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/oac-tree-server/automation_server.h>
#include <sup/oac-tree-server/exceptions.h>
#include <sup/oac-tree-server/server_job_info_io.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/oac-tree/procedure.h>
#include <sup/oac-tree/workspace.h>

namespace sup
{
namespace oac_tree_server
{
using sup::oac_tree::LocalJob;

AutomationServer::AutomationServer(const std::string& server_prefix,
                                   IAnyValueManagerRegistry& av_mgr_registry)
  : m_server_prefix{server_prefix}
  , m_av_mgr_registry{av_mgr_registry}
  , m_job_info_ios{}
  , m_jobs{}
  , m_mtx{}
{}

AutomationServer::~AutomationServer() = default;

void AutomationServer::AddJob(std::unique_ptr<sup::oac_tree::Procedure> proc)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  auto idx = m_jobs.size();
  auto job_prefix = CreateJobPrefix(m_server_prefix, idx);
  auto n_vars = GetNumberOfVariables(*proc);
  auto job_info_io = std::make_unique<ServerJobInfoIO>(job_prefix, n_vars,
                                                       m_av_mgr_registry.GetAnyValueManager(idx));
  (void)m_job_info_ios.emplace_back(std::move(job_info_io));
  (void)m_jobs.emplace_back(std::move(proc), *m_job_info_ios.back());
}

std::string AutomationServer::GetServerPrefix() const
{
  return m_server_prefix;
}

sup::dto::uint32 AutomationServer::GetNumberOfJobs() const
{
  std::lock_guard<std::mutex> lk{m_mtx};
  return m_jobs.size();
}

sup::oac_tree::JobInfo AutomationServer::GetJobInfo(sup::dto::uint32 job_idx) const
{
  auto& job = GetJob(job_idx);
  return job.GetInfo();
}

void AutomationServer::EditBreakpoint(sup::dto::uint32 job_idx, sup::dto::uint32 instr_idx,
                                      bool breakpoint_active)
{
  auto& job = GetJob(job_idx);
  if (breakpoint_active)
  {
    job.SetBreakpoint(instr_idx);
  }
  else
  {
    job.RemoveBreakpoint(instr_idx);
  }
  return;
}

void AutomationServer::SendJobCommand(sup::dto::uint32 job_idx, sup::oac_tree::JobCommand command)
{
  using sup::oac_tree::JobCommand;
  auto& job = GetJob(job_idx);
  switch (command)
  {
  case JobCommand::kStart:
    job.Start();
    break;
  case JobCommand::kStep:
    job.Step();
    break;
  case JobCommand::kPause:
    job.Pause();
    break;
  case JobCommand::kReset:
    job.Reset();
    break;
  case JobCommand::kHalt:
    job.Halt();
    break;
  default:
    {
      const std::string error = "AutomationServer::GetJobInfo(): unknown command enumerator ["
        + std::to_string(static_cast<int>(command)) + "]";
      throw InvalidOperationException(error);
    }
  }
}

LocalJob& AutomationServer::GetJob(sup::dto::uint32 job_idx)
{
  return const_cast<LocalJob&>(const_cast<const AutomationServer*>(this)->GetJob(job_idx));
}

const LocalJob& AutomationServer::GetJob(sup::dto::uint32 job_idx) const
{
  std::lock_guard<std::mutex> lk{m_mtx};
  if (job_idx >= m_jobs.size())
  {
    const std::string error = "AutomationServer::GetJob(): index out of bounds; requesting"
      + std::to_string(job_idx) + " out of " + std::to_string(m_jobs.size()) + " jobs";
    throw InvalidOperationException(error);
  }
  return m_jobs[job_idx];
}

sup::dto::uint32 GetNumberOfVariables(const sup::oac_tree::Procedure& proc)
{
  const auto& ws = proc.GetWorkspace();
  auto var_names = ws.VariableNames();
  return var_names.size();
}

}  // namespace oac_tree_server

}  // namespace sup
