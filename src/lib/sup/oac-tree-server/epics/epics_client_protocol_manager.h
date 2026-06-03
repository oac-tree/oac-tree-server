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

#ifndef SUP_OAC_TREE_SERVER_EPICS_CLIENT_PROTOCOL_MANAGER_H_
#define SUP_OAC_TREE_SERVER_EPICS_CLIENT_PROTOCOL_MANAGER_H_

#include <sup/oac-tree-server/i_client_protocol_manager.h>

#include <memory>

namespace sup
{
namespace oac_tree_server
{
/**
  * @brief Client protocol manager implementation that uses EPICS as the network backend.
 */
class EPICSClientProtocolManager final : public IClientProtocolManager
{
public:
  EPICSClientProtocolManager(const std::string& service_name);
 ~EPICSClientProtocolManager() override;

  // No copy or move
  EPICSClientProtocolManager(const EPICSClientProtocolManager& other) = delete;
  EPICSClientProtocolManager(EPICSClientProtocolManager&& other) = delete;
  EPICSClientProtocolManager& operator=(const EPICSClientProtocolManager& other) = delete;
  EPICSClientProtocolManager& operator=(EPICSClientProtocolManager&& other) = delete;

  IJobManager& GetJobManager() override;

  std::unique_ptr<sup::oac_tree::IJob> CreateJob(
    sup::dto::uint32 job_idx, sup::oac_tree::IJobInfoIO &job_info_io) override;

private:
  std::string m_service_name;
  std::unique_ptr<IJobManager> m_job_manager;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_EPICS_CLIENT_PROTOCOL_MANAGER_H_
