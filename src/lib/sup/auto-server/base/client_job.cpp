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
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/auto-server/client_job.h>

#include <sup/auto-server/anyvalue_io_helper.h>
#include <sup/auto-server/client_anyvalue_manager.h>
#include <sup/auto-server/exceptions.h>

namespace sup
{
namespace auto_server
{
class ClientJobImpl
{
public:
  ClientJobImpl(IJobManager& job_manager, sup::dto::uint32 job_idx,
                const AnyValueIOFactoryFunction& factory_func,
                sup::sequencer::IJobInfoIO& job_info_io);
  ~ClientJobImpl();

  IJobManager& GetJobManager();

  sup::dto::uint32 GetJobIndex() const;

  const sup::sequencer::JobInfo& GetInfo() const;

private:
  IJobManager& m_job_manager;
  sup::dto::uint32 m_job_idx;
  ClientAnyValueManager m_av_mgr;
  std::unique_ptr<IAnyValueIO> m_anyvalue_io;
  std::unique_ptr<sup::sequencer::JobInfo> m_job_info;
};

ClientJob::ClientJob(IJobManager& job_manager, sup::dto::uint32 job_idx,
                     const AnyValueIOFactoryFunction& factory_func,
                     sup::sequencer::IJobInfoIO& job_info_io)
  : IJob{}
  , m_impl{new ClientJobImpl{job_manager, job_idx, factory_func, job_info_io}}
{}

ClientJob::~ClientJob() = default;

ClientJob::ClientJob(ClientJob&& other)
  : IJob{}
  , m_impl{std::move(other.m_impl)}
{}

ClientJob& ClientJob::operator=(ClientJob&& other)
{
  ClientJob moved{std::move(other)};
  std::swap(m_impl, moved.m_impl);
  return *this;
}

const sup::sequencer::JobInfo& ClientJob::GetInfo() const
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
  using sup::sequencer::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kStart);
}

void ClientJob::Step()
{
  using sup::sequencer::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kStep);
}

void ClientJob::Pause()
{
  using sup::sequencer::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kPause);
}

void ClientJob::Reset()
{
  using sup::sequencer::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kReset);
}

void ClientJob::Halt()
{
  using sup::sequencer::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kHalt);
}

std::unique_ptr<sup::sequencer::IJob> CreateClientJob(
    IJobManager &job_manager, sup::dto::uint32 job_idx,
    const AnyValueIOFactoryFunction &factory_func, sup::sequencer::IJobInfoIO &job_info_io)
{
  std::unique_ptr<sup::sequencer::IJob> result{};
  try
  {
    result.reset(new ClientJob{job_manager, job_idx, factory_func, job_info_io});
  }
  catch(const MessageException& e)
  {
    // ignore
  }
  return result;
}

ClientJobImpl::ClientJobImpl(IJobManager& job_manager, sup::dto::uint32 job_idx,
                             const AnyValueIOFactoryFunction& factory_func,
                             sup::sequencer::IJobInfoIO& job_info_io)
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
  m_job_info.reset(new sup::sequencer::JobInfo(m_job_manager.GetJobInfo(job_idx)));
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

const sup::sequencer::JobInfo& ClientJobImpl::GetInfo() const
{
  return *m_job_info;
}

}  // namespace auto_server

}  // namespace sup
