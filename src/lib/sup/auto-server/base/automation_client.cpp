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

#include <sup/auto-server/automation_client.h>

#include <sup/auto-server/exceptions.h>

namespace sup
{
namespace auto_server
{

AutomationClient::AutomationClient(IJobManager& job_manager,
                                   const AnyValueIOFactoryFunction& factory_func)
  : m_job_manager{job_manager}
  , m_factory_func{factory_func}
  , m_jobs{}
{}

AutomationClient::~AutomationClient() = default;

bool AutomationClient::Connect(std::size_t job_idx, IJobInfoIO& job_info_io)
{
  if (m_jobs.find(job_idx) != m_jobs.end())
  {
    return false;
  }
  try
  {
    const auto& job_info = m_job_manager.GetJobInfo(job_idx);
    m_jobs[job_idx] =
      std::unique_ptr<ClientJob>(new ClientJob{job_info, job_info_io, m_factory_func});
  }
  catch(const InvalidOperationException&)
  {
    return false;
  }
  return true;
}

bool AutomationClient::Disconnect(std::size_t job_idx)
{
  return m_jobs.erase(job_idx) == 1u;
}

}  // namespace auto_server

}  // namespace sup
