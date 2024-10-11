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

#include <sup/auto-server/client_job.h>

#include <sup/auto-server/client_anyvalue_manager.h>
#include <sup/auto-server/listener_initializer.h>

namespace sup
{
namespace auto_server
{
class ClientJobImpl
{
public:
  ClientJobImpl(const JobInfo& job_info, IJobInfoIO& job_info_io,
                const ListenerFactoryFunction& factory_func);
  ~ClientJobImpl();

private:
  ClientAnyValueManager m_av_mgr;
  ListenerInitializer m_listener;
};

ClientJob::ClientJob(const JobInfo& job_info, IJobInfoIO& job_info_io,
                     const ListenerFactoryFunction& factory_func)
  : m_impl{new ClientJobImpl{job_info, job_info_io, factory_func}}
{}

ClientJob::~ClientJob() = default;

ClientJob::ClientJob(ClientJob&&) = default;

ClientJob& ClientJob::operator=(ClientJob&& other) = default;

ClientJobImpl::ClientJobImpl(const JobInfo& job_info, IJobInfoIO& job_info_io,
                             const ListenerFactoryFunction& factory_func)
  : m_av_mgr{job_info_io}
  , m_listener{job_info.GetPrefix(), job_info.GetNumberOfVariables(), factory_func(m_av_mgr)}
{
  m_listener.InitNumberOfInstructions(job_info.GetNumberOfInstructions());
}

ClientJobImpl::~ClientJobImpl() = default;

}  // namespace auto_server

}  // namespace sup
