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
 * @brief AutomationClient wraps an IJobManager implementation and allows to connect custom
 * IJobInfoIO implementations to jobs that are managed by the IJobManager.
 */
class AutomationClient
{
public:
  /**
   * @brief AutomationClient constructor with injection of the wrapped IJobManager implementation
   * and a factory function to allow connecting custom IJobInfoIO implementations to the managed
   * jobs.
   *
   * @param job_manager IJobManager implementation that provides static information about multiple jobs.
   * @param factory_func Factory function to use for the creation of listener objects.
   */
  AutomationClient(IJobManager& job_manager, const ListenerFactoryFunction& factory_func);

  /**
   * @brief Destructor.
   */
  ~AutomationClient();

  /**
   * @brief Connect status updates and user input/output of a remote job to a custom provided
   * IJobInfoIO implementation.
   *
   * @param job_idx Index of job to connect.
   * @param job_info_io Custom IJobInfoIO implementation.
   * @return True when successful. Failure typically indicates that the job was already connected or
   * the client could not retrieve the required information from the IJobManager.
   */
  bool Connect(std::size_t job_idx, IJobInfoIO& job_info_io);

  /**
   * @brief Disconnect a previously connected IJobInfoIO implementation.
   *
   * @param job_idx Index of job to disconnect.
   * @return True when successful. Failure typically means the job was not connected previously.
   */
  bool Disconnect(std::size_t job_idx);

private:
  IJobManager& m_job_manager;
  const ListenerFactoryFunction& m_factory_func;
  std::map<std::size_t, std::unique_ptr<ClientJob>> m_jobs;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_AUTOMATION_CLIENT_H_
