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
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include "ws_client_job.h"

#include <sup/oac-tree-server/exceptions.h>

namespace sup
{
namespace oac_tree_server
{

WSClientJob::WSClientJob(IJobManager& job_manager, sup::dto::uint32 job_idx,
                         const std::string& host_name, unsigned short port)
  : IJob{}
  , m_job_manager{std::addressof(job_manager)}
  , m_job_idx{job_idx}
  , m_job_info{m_job_manager->GetJobInfo(job_idx)}
{
  auto n_jobs = m_job_manager->GetNumberOfJobs();
  if (job_idx >= n_jobs)
  {
    const std::string error = "WSClientJob ctor: trying to create job with index ["
      + std::to_string(job_idx) + "] out of [" + std::to_string(n_jobs) + "] jobs";
    throw InvalidOperationException(error);
  }
  (void)host_name;
  (void)port;
}

WSClientJob::~WSClientJob() = default;

WSClientJob::WSClientJob(WSClientJob&& other) noexcept
  : IJob{}
  , m_job_manager{other.m_job_manager}
  , m_job_idx{other.m_job_idx}
  , m_job_info{other.m_job_info}
{}

WSClientJob& WSClientJob::operator=(WSClientJob&& other) noexcept
{
  if (this != &other)
  {
    m_job_manager = other.m_job_manager;
    m_job_idx = other.m_job_idx;
    m_job_info = other.m_job_info;
  }
  return *this;
}

const sup::oac_tree::JobInfo& WSClientJob::GetInfo() const
{
  return m_job_info;
}

void WSClientJob::SetBreakpoint(sup::dto::uint32 instr_idx)
{
  m_job_manager->EditBreakpoint(m_job_idx, instr_idx, true);
}

void WSClientJob::RemoveBreakpoint(sup::dto::uint32 instr_idx)
{
  m_job_manager->EditBreakpoint(m_job_idx, instr_idx, false);
}

void WSClientJob::Start()
{
  using sup::oac_tree::JobCommand;
  m_job_manager->SendJobCommand(m_job_idx, JobCommand::kStart);
}

void WSClientJob::Step()
{
  using sup::oac_tree::JobCommand;
  m_job_manager->SendJobCommand(m_job_idx, JobCommand::kStep);
}

void WSClientJob::Pause()
{
  using sup::oac_tree::JobCommand;
  m_job_manager->SendJobCommand(m_job_idx, JobCommand::kPause);
}

void WSClientJob::Reset()
{
  using sup::oac_tree::JobCommand;
  m_job_manager->SendJobCommand(m_job_idx, JobCommand::kReset);
}

void WSClientJob::Halt()
{
  using sup::oac_tree::JobCommand;
  m_job_manager->SendJobCommand(m_job_idx, JobCommand::kHalt);
}

}  // namespace oac_tree_server

}  // namespace sup
