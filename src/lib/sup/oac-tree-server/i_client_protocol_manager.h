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

#ifndef SUP_OAC_TREE_SERVER_I_CLIENT_PROTOCOL_MANAGER_H_
#define SUP_OAC_TREE_SERVER_I_CLIENT_PROTOCOL_MANAGER_H_

#include <sup/oac-tree-server/i_job_manager.h>

#include <sup/oac-tree/i_job_info_io.h>
#include <sup/oac-tree/i_job.h>

namespace sup
{
namespace oac_tree_server
{
/**
  * @brief Interface for client protocol manager.
 */
class IClientProtocolManager
{
public:
  virtual ~IClientProtocolManager();

  IClientProtocolManager(const IClientProtocolManager& other) = delete;
  IClientProtocolManager(IClientProtocolManager&& other) = delete;
  IClientProtocolManager& operator=(const IClientProtocolManager& other) = delete;
  IClientProtocolManager& operator=(IClientProtocolManager&& other) = delete;

  /**
   * @brief Get a reference to an IJobManager object.
   */
  virtual IJobManager& GetJobManager() = 0;

  /**
   * @brief Create a client job that will forward all updates and user IO to the passed IJobInfoIO.
   */
  virtual std::unique_ptr<sup::oac_tree::IJob> CreateJob(
    sup::dto::uint32 job_idx, sup::oac_tree::IJobInfoIO &job_info_io) = 0;

protected:
  IClientProtocolManager() = default;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_I_CLIENT_PROTOCOL_MANAGER_H_
