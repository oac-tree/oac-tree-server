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

#include "epics_client_job.h"

#include <sup/oac-tree-server/base/anyvalue_io_helper.h>
#include <sup/oac-tree-server/base/client_anyvalue_manager.h>

#include <sup/oac-tree-server/exceptions.h>

#include <chrono>
#include <thread>

namespace sup
{
namespace oac_tree_server
{
class EPICSClientJobImpl
{
public:
  EPICSClientJobImpl(IJobManager& job_manager, sup::dto::uint32 job_idx,
                     const AnyValueIOFactoryFunction& factory_func,
                     sup::oac_tree::IJobInfoIO& job_info_io);
  ~EPICSClientJobImpl();

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

EPICSClientJob::EPICSClientJob(IJobManager& job_manager, sup::dto::uint32 job_idx,
                               const AnyValueIOFactoryFunction& factory_func,
                               sup::oac_tree::IJobInfoIO& job_info_io)
  : IClientJob{}
  , m_impl{std::make_unique<EPICSClientJobImpl>(job_manager, job_idx, factory_func, job_info_io)}
{}

EPICSClientJob::~EPICSClientJob() { m_impl.reset(); }

EPICSClientJob::EPICSClientJob(EPICSClientJob&& other) noexcept
  : IClientJob{}
  , m_impl{std::move(other.m_impl)}
{}

EPICSClientJob& EPICSClientJob::operator=(EPICSClientJob&& other) noexcept
{
  EPICSClientJob moved{std::move(other)};
  std::swap(m_impl, moved.m_impl);
  return *this;
}

const sup::oac_tree::JobInfo& EPICSClientJob::GetInfo() const
{
  return m_impl->GetInfo();
}

void EPICSClientJob::SetBreakpoint(sup::dto::uint32 instr_idx)
{
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.EditBreakpoint(job_idx, instr_idx, true);
}

void EPICSClientJob::RemoveBreakpoint(sup::dto::uint32 instr_idx)
{
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.EditBreakpoint(job_idx, instr_idx, false);
}

void EPICSClientJob::Start()
{
  using sup::oac_tree::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kStart);
}

void EPICSClientJob::Step()
{
  using sup::oac_tree::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kStep);
}

void EPICSClientJob::Pause()
{
  using sup::oac_tree::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kPause);
}

void EPICSClientJob::Reset()
{
  using sup::oac_tree::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kReset);
}

void EPICSClientJob::Halt()
{
  using sup::oac_tree::JobCommand;
  auto& job_manager = m_impl->GetJobManager();
  auto job_idx = m_impl->GetJobIndex();
  job_manager.SendJobCommand(job_idx, JobCommand::kHalt);
}

bool EPICSClientJob::WaitForConnection(std::chrono::milliseconds timeout)
{
  return WaitForServerConnection(m_impl->GetJobManager(), timeout);
}

std::unique_ptr<IClientJob> CreateEPICSClientJob(
    IJobManager &job_manager, sup::dto::uint32 job_idx,
    const AnyValueIOFactoryFunction &factory_func, sup::oac_tree::IJobInfoIO &job_info_io)
{
  std::unique_ptr<IClientJob> result{};
  try
  {
    result = std::make_unique<EPICSClientJob>(job_manager, job_idx, factory_func, job_info_io);
  }
  catch(const MessageException& e)
  {
    // ignore
  }
  return result;
}

bool WaitForServerConnection(IJobManager& job_manager, std::chrono::milliseconds timeout)
{
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  do
  {
    try
    {
      (void)job_manager.GetServerPrefix();
      return true;
    }
    catch (const MessageException&)
    {
      // Server not reachable (yet); keep probing until the deadline.
    }
    // Guard against busy-looping in case a probe ever fails quickly.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  } while (std::chrono::steady_clock::now() < deadline);
  return false;
}

EPICSClientJobImpl::EPICSClientJobImpl(IJobManager& job_manager, sup::dto::uint32 job_idx,
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
    const std::string error = "EPICSClientJob ctor: trying to create job with index ["
      + std::to_string(job_idx) + "] out of [" + std::to_string(n_jobs) + "] jobs";
    throw InvalidOperationException(error);
  }
  auto server_prefix = m_job_manager.GetServerPrefix();
  auto job_prefix = CreateJobPrefix(server_prefix, job_idx);
  m_job_info = std::make_unique<sup::oac_tree::JobInfo>(m_job_manager.GetJobInfo(job_idx));
  InitializeJobAndVariables(*m_anyvalue_io, job_prefix, m_job_info->GetNumberOfVariables());
  InitializeInstructions(*m_anyvalue_io, job_prefix, m_job_info->GetNumberOfInstructions());
}

EPICSClientJobImpl::~EPICSClientJobImpl() = default;

IJobManager& EPICSClientJobImpl::GetJobManager()
{
  return m_job_manager;
}

sup::dto::uint32 EPICSClientJobImpl::GetJobIndex() const
{
  return m_job_idx;
}

const sup::oac_tree::JobInfo& EPICSClientJobImpl::GetInfo() const
{
  return *m_job_info;
}

}  // namespace oac_tree_server

}  // namespace sup
