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

class WSClientJobImpl
{
public:
  WSClientJobImpl(IJobManager& job_manager, sup::dto::uint32 job_idx,
                 const std::string& host_name, unsigned short port);

  IJobManager& GetJobManager();
  sup::dto::uint32 GetJobIndex() const;
  const sup::oac_tree::JobInfo& GetInfo() const;

private:
  IJobManager& m_job_manager;
  sup::dto::uint32 m_job_idx;
  std::unique_ptr<sup::oac_tree::JobInfo> m_job_info;
};

WSClientJobImpl::WSClientJobImpl(IJobManager &job_manager, sup::dto::uint32 job_idx,
                                const std::string& host_name, unsigned short port)
  : m_job_manager{job_manager}
  , m_job_idx{job_idx}
  , m_job_info{}
{
  auto n_jobs = m_job_manager.GetNumberOfJobs();
  if (job_idx >= n_jobs)
  {
    const std::string error = "WSClientJob ctor: trying to create job with index ["
      + std::to_string(job_idx) + "] out of [" + std::to_string(n_jobs) + "] jobs";
    throw InvalidOperationException(error);
  }
  m_job_info = std::make_unique<sup::oac_tree::JobInfo>(m_job_manager.GetJobInfo(job_idx));
  (void)host_name;
  (void)port;
}

IJobManager& WSClientJobImpl::GetJobManager()
{
  return m_job_manager;
}

sup::dto::uint32 WSClientJobImpl::GetJobIndex() const
{
  return m_job_idx;
}

const sup::oac_tree::JobInfo& WSClientJobImpl::GetInfo() const
{
  return *m_job_info;
}

WSClientJob::WSClientJob(IJobManager &job_manager, sup::dto::uint32 job_idx,
                         const std::string& host_name, unsigned short port)
  : IJob{}
  , m_impl{std::make_unique<WSClientJobImpl>(job_manager, job_idx, host_name, port)}
{}

WSClientJob::~WSClientJob() = default;

WSClientJob::WSClientJob(WSClientJob&& other) noexcept
  : IJob{}
  , m_impl{std::move(other.m_impl)}
{}

WSClientJob& WSClientJob::operator=(WSClientJob&& other) noexcept
{
  if (this != &other)
  {
    m_impl = std::move(other.m_impl);
  }
  return *this;
}

const sup::oac_tree::JobInfo& WSClientJob::GetInfo() const
{
  return m_impl->GetInfo();
}

void WSClientJob::SetBreakpoint(sup::dto::uint32 instr_idx)
{
  m_impl->GetJobManager().EditBreakpoint(m_impl->GetJobIndex(), instr_idx, true);
}

void WSClientJob::RemoveBreakpoint(sup::dto::uint32 instr_idx)
{
  m_impl->GetJobManager().EditBreakpoint(m_impl->GetJobIndex(), instr_idx, false);
}

void WSClientJob::Start()
{
  using sup::oac_tree::JobCommand;
  m_impl->GetJobManager().SendJobCommand(m_impl->GetJobIndex(), JobCommand::kStart);
}

void WSClientJob::Step()
{
  using sup::oac_tree::JobCommand;
  m_impl->GetJobManager().SendJobCommand(m_impl->GetJobIndex(), JobCommand::kStep);
}

void WSClientJob::Pause()
{
  using sup::oac_tree::JobCommand;
  m_impl->GetJobManager().SendJobCommand(m_impl->GetJobIndex(), JobCommand::kPause);
}

void WSClientJob::Reset()
{
  using sup::oac_tree::JobCommand;
  m_impl->GetJobManager().SendJobCommand(m_impl->GetJobIndex(), JobCommand::kReset);
}

void WSClientJob::Halt()
{
  using sup::oac_tree::JobCommand;
  m_impl->GetJobManager().SendJobCommand(m_impl->GetJobIndex(), JobCommand::kHalt);
}

}  // namespace oac_tree_server

}  // namespace sup
