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

#ifndef SUP_OAC_TREE_SERVER_WS_MAIN_SERVER_H_
#define SUP_OAC_TREE_SERVER_WS_MAIN_SERVER_H_

#include <sup/oac-tree-server/i_server_protocol_factory.h>

#include <simple-web/server_ws.hpp>

#include <sup/protocol/protocol_factory.h>

#include <future>
#include <memory>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief WSMainServer is the WebSocket implementation of an RPC server that exposes control and
 * information requests for a job manager.
 */
class WSMainServer final : public IMainServer
{
public:
  using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

  explicit WSMainServer(unsigned short port, IJobManager& job_manager);
  ~WSMainServer();

  // No copy or move
  WSMainServer(const WSMainServer& other) = delete;
  WSMainServer(WSMainServer&& other) = delete;
  WSMainServer& operator=(const WSMainServer& other) = delete;
  WSMainServer& operator=(WSMainServer&& other) = delete;

private:
  IJobManager& m_job_manager;
  WsServer m_server;
  std::future<void> m_server_thread;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_MAIN_SERVER_H_
