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
#include <sup/auto-server/anyvalue_io_helper.h>

namespace sup
{
namespace auto_server
{
class ClientJobImpl
{
public:
  ClientJobImpl(const sup::sequencer::JobInfo& job_info, const std::string& job_prefix,
                sup::sequencer::IJobInfoIO& job_info_io,
                const AnyValueIOFactoryFunction& factory_func);
  ~ClientJobImpl();

private:
  ClientAnyValueManager m_av_mgr;
  std::unique_ptr<IAnyValueIO> m_anyvalue_io;
};

ClientJob::ClientJob(const sup::sequencer::JobInfo& job_info, const std::string& job_prefix,
                     sup::sequencer::IJobInfoIO& job_info_io,
                     const AnyValueIOFactoryFunction& factory_func)
  : m_impl{new ClientJobImpl{job_info, job_prefix, job_info_io, factory_func}}
{}

ClientJob::~ClientJob() = default;

ClientJob::ClientJob(ClientJob&&) = default;

ClientJob& ClientJob::operator=(ClientJob&& other) = default;

ClientJobImpl::ClientJobImpl(const sup::sequencer::JobInfo& job_info, const std::string& job_prefix,
                             sup::sequencer::IJobInfoIO& job_info_io,
                             const AnyValueIOFactoryFunction& factory_func)
  : m_av_mgr{job_info_io}
  , m_anyvalue_io{factory_func(m_av_mgr)}
{
  InitializeJobAndVariables(*m_anyvalue_io, job_prefix, job_info.GetNumberOfVariables());
  InitializeInstructions(*m_anyvalue_io, job_prefix, job_info.GetNumberOfInstructions());
}

ClientJobImpl::~ClientJobImpl() = default;

}  // namespace auto_server

}  // namespace sup
