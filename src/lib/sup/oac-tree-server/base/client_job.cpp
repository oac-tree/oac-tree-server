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

#include <sup/oac-tree-server/client_job.h>

#include <sup/oac-tree-server/anyvalue_io_helper.h>
#include <sup/oac-tree-server/client_anyvalue_manager.h>
#include <sup/oac-tree-server/exceptions.h>

namespace sup
{
namespace oac_tree_server
{
class ClientJobImpl
{
public:
  ClientJobImpl(IJobManager& job_manager, sup::dto::uint32 job_idx,
                const AnyValueIOFactoryFunction& factory_func,
                sup::oac_tree::IJobInfoIO& job_info_io);
  ~ClientJobImpl();

  IJobManager& GetJobManager();

  sup::dto::uint32 GetJobIndex() const;

  const sup::oac_tree::JobInfo& GetInfo() const;

private:
  IJobManager& m_job_manager;
  sup::dto::uint32 m_job_idx;
  ClientAnyValueManager m_av_mgr;
  std::unique_ptr<IAnyValueIO> m_anyvalue_io;
  std::unique_ptr<sup::oac_tree::JobInfo> m_job_info;
};

ClientJob::ClientJob(IJobManager& job_manager, sup::dto::uint32 job_idx,
                     const AnyValueIOFactoryFunction& factory_func,
                     sup::oac_tree::IJobInfoIO& job_info_io)
  : IJob{}
  , m_impl{std::make_unique<ClientJobImpl>(job_manager, job_idx, factory_func, job_info_io)}
{}

ClientJob::~ClientJob() = default;

ClientJob::ClientJob(ClientJob&& other) noexcept
  : IJob{}
  , m_impl{std::move(other.m_impl)}
{}

ClientJob& ClientJob::operator=(ClientJob&& other) noexcept
{
  ClientJob moved{std::move(other)};
  std::swap(m_impl, moved.m_impl);
  return *this;
}

const sup::oac_tree::JobInfo& ClientJob::GetInfo() const
{
  return m_impl->GetInfo();
}

void ClientJob::SetBreakpoint(sup::dto::uint32 instr_idx)
{
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.EditBreakpoint(job_idx, instr_idx, true);
}

void ClientJob::RemoveBreakpoint(sup::dto::uint32 instr_idx)
{
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.EditBreakpoint(job_idx, instr_idx, false);
}

void ClientJob::Start()
{
  using sup::oac_tree::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kStart);
}

void ClientJob::Step()
{
  using sup::oac_tree::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kStep);
}

void ClientJob::Pause()
{
  using sup::oac_tree::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kPause);
}

void ClientJob::Reset()
{
  using sup::oac_tree::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kReset);
}

void ClientJob::Halt()
{
  using sup::oac_tree::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kHalt);
}

std::unique_ptr<sup::oac_tree::IJob> CreateClientJob(
    IJobManager &job_manager, sup::dto::uint32 job_idx,
    const AnyValueIOFactoryFunction &factory_func, sup::oac_tree::IJobInfoIO &job_info_io)
{
  std::unique_ptr<sup::oac_tree::IJob> result{};
  try
  {
    result = std::make_unique<ClientJob>(job_manager, job_idx, factory_func, job_info_io);
  }
  catch(const MessageException& e)
  {
    // ignore
  }
  return result;
}

ClientJobImpl::ClientJobImpl(IJobManager& job_manager, sup::dto::uint32 job_idx,
                             const AnyValueIOFactoryFunction& factory_func,
                             sup::oac_tree::IJobInfoIO& job_info_io)
  : m_job_manager{job_manager}
  , m_job_idx{job_idx}
  , m_av_mgr{job_info_io}
  , m_anyvalue_io{factory_func(m_av_mgr)}
  , m_job_info{}
{
  auto n_jobs = m_job_manager.GetNumberOfJobs();
  if (job_idx >= n_jobs)
  {
    const std::string error = "ClientJob ctor: trying to create job with index ["
      + std::to_string(job_idx) + "] out of [" + std::to_string(n_jobs) + "] jobs";
    throw InvalidOperationException(error);
  }
  auto server_prefix = m_job_manager.GetServerPrefix();
  auto job_prefix = CreateJobPrefix(server_prefix, job_idx);
  m_job_info = std::make_unique<sup::oac_tree::JobInfo>(m_job_manager.GetJobInfo(job_idx));
  InitializeJobAndVariables(*m_anyvalue_io, job_prefix, m_job_info->GetNumberOfVariables());
  InitializeInstructions(*m_anyvalue_io, job_prefix, m_job_info->GetNumberOfInstructions());
}

ClientJobImpl::~ClientJobImpl() = default;

IJobManager& ClientJobImpl::GetJobManager()
{
  return m_job_manager;
}

sup::dto::uint32 ClientJobImpl::GetJobIndex() const
{
  return m_job_idx;
}

const sup::oac_tree::JobInfo& ClientJobImpl::GetInfo() const
{
  return *m_job_info;
}

}  // namespace oac_tree_server

}  // namespace sup
