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

#ifndef SUP_AUTO_SERVER_AUTOMATION_CLIENT_H_
#define SUP_AUTO_SERVER_AUTOMATION_CLIENT_H_

#include <sup/auto-server/client_job.h>
#include <sup/auto-server/i_job_manager.h>
#include <sup/auto-server/i_anyvalue_listener.h>
#include <sup/auto-server/i_job_info_io.h>

#include <map>
#include <memory>

namespace sup
{
namespace auto_server
{
/**
 * @brief AutomationClient.
 */
class AutomationClient
{
public:
  AutomationClient(IJobManager& job_manager, const ListenerFactoryFunction& factory_func);
  ~AutomationClient();

  bool Connect(std::size_t job_idx, IJobInfoIO& job_info_io);
  bool Disconnect(std::size_t job_idx);

private:
  IJobManager& m_job_manager;
  const ListenerFactoryFunction& m_factory_func;
  std::map<std::size_t, std::unique_ptr<ClientJob>> m_jobs;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_AUTOMATION_CLIENT_H_
