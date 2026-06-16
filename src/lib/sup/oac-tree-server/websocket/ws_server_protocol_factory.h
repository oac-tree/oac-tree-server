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

#ifndef SUP_OAC_TREE_SERVER_WS_SERVER_PROTOCOL_FACTORY_H_
#define SUP_OAC_TREE_SERVER_WS_SERVER_PROTOCOL_FACTORY_H_

#include <sup/oac-tree-server/i_server_protocol_factory.h>

#include <chrono>

namespace sup
{
namespace oac_tree_server
{
class WSMainServer;

/**
  * @brief Server protocol factory implementation that uses websockets as the network backend.
 */
class WSServerProtocolFactory final : public IServerProtocolFactory
{
public:
  WSServerProtocolFactory(unsigned short port, std::chrono::milliseconds resend_interval);
 ~WSServerProtocolFactory() override;

  // No copy or move
  WSServerProtocolFactory(const WSServerProtocolFactory& other) = delete;
  WSServerProtocolFactory(WSServerProtocolFactory&& other) = delete;
  WSServerProtocolFactory& operator=(const WSServerProtocolFactory& other) = delete;
  WSServerProtocolFactory& operator=(WSServerProtocolFactory&& other) = delete;

  std::string GetServerId() const override;

  std::unique_ptr<IMainServer> CreateMainServer(IJobManager& job_manager) override;

  std::unique_ptr<sup::oac_tree::IJobInfoIO> CreateJobInfoIO(
    sup::dto::uint32 job_idx, sup::dto::uint32 n_vars) override;

private:
  std::unique_ptr<WSMainServer> m_main_server;
  std::chrono::milliseconds m_resend_interval;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_SERVER_PROTOCOL_FACTORY_H_
