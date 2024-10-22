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

#ifndef SUP_AUTO_SERVER_I_CLIENT_JOB_H_
#define SUP_AUTO_SERVER_I_CLIENT_JOB_H_

#include <sup/auto-server/i_anyvalue_io.h>

#include <sup/sequencer/i_job_info_io.h>
#include <sup/sequencer/job_info.h>

#include <memory>

namespace sup
{
namespace auto_server
{
class ClientJobImpl;
/**
 * @brief ClientJob creates and manages the different components required to monitor and interact
 * with a job on the client side. It uses a provided IJobInfoIO object to handle all updates and
 * input/output requests.
 */
class ClientJob
{
public:
  ClientJob(const sup::sequencer::JobInfo& job_info, const std::string& job_prefix,
            sup::sequencer::IJobInfoIO& job_info_io,
            const AnyValueIOFactoryFunction& factory_func);
  ~ClientJob();

  ClientJob(ClientJob&& other);
  ClientJob& operator=(ClientJob&& other);

private:
  std::unique_ptr<ClientJobImpl> m_impl;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_CLIENT_JOB_H_
