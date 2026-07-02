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

#ifndef SUP_OAC_TREE_SERVER_I_CLIENT_JOB_MANAGER_H_
#define SUP_OAC_TREE_SERVER_I_CLIENT_JOB_MANAGER_H_

#include <sup/oac-tree-server/i_client_job.h>
#include <sup/oac-tree-server/i_job_manager.h>

#include <sup/oac-tree/i_job_info_io.h>

#include <chrono>

namespace sup
{
namespace oac_tree_server
{
/**
  * @brief Interface for a client job manager.
 */
class IClientJobManager : public IJobManager
{
public:
  virtual ~IClientJobManager();

  IClientJobManager(const IClientJobManager& other) = delete;
  IClientJobManager(IClientJobManager&& other) = delete;
  IClientJobManager& operator=(const IClientJobManager& other) = delete;
  IClientJobManager& operator=(IClientJobManager&& other) = delete;

  /**
   * @brief Create a client job that will forward all updates and user IO to the passed IJobInfoIO.
   * @param job_idx Index that identifies a single job.
   * @param job_info_io IJobInfoIO instance to which the created client job will forward all
   * updates and user IO.
   * @return A unique pointer to the created client job.
   */
  virtual std::unique_ptr<IClientJob> CreateJob(
    sup::dto::uint32 job_idx, sup::oac_tree::IJobInfoIO& job_info_io) = 0;

  /**
   * @brief Wait for a connection to be established within the specified timeout.
   * @param timeout The maximum duration in milliseconds to wait for a connection.
   * @return true if the connection was established within the timeout, false otherwise.
   */
  virtual bool WaitForConnection(std::chrono::milliseconds timeout) = 0;

protected:
  IClientJobManager() = default;
};

bool IsConnected(IClientJobManager& client_mgr);

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_I_CLIENT_JOB_MANAGER_H_
