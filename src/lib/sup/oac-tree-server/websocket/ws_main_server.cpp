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

#include "ws_main_server.h"

#include <sup/oac-tree-server/ws_message_handler.h>

namespace sup
{
namespace oac_tree_server
{

WSMainServer::WSMainServer(unsigned short port, IJobManager& job_manager)
  : IMainServer{}
  , m_job_manager{job_manager}
  , m_server{}
  , m_server_thread{}
{
  m_server.config.port = port;
  auto &endpoint = m_server.endpoint["^/info/?$"];

  endpoint.on_message = [this](std::shared_ptr<WsServer::Connection> connection,
                               std::shared_ptr<WsServer::InMessage> in_message) {
    auto out_message = HandleJobManagerMessage(in_message->string(), m_job_manager);
    if (!out_message.empty())
    {
      connection->send(out_message);
    }
  };

  endpoint.on_open = [](std::shared_ptr<WsServer::Connection> connection) {
    std::cout << "Server: Opened connection " << connection.get() << std::endl;
  };

  // See RFC 6455 7.4.1. for status codes
  endpoint.on_close = [](std::shared_ptr<WsServer::Connection> connection, int status,
                               const std::string & /*reason*/) {
    std::cout << "Server: Closed connection " << connection.get() << " with status code "
              << status << std::endl;
  };

  // Can modify handshake response headers here if needed
  endpoint.on_handshake = [](std::shared_ptr<WsServer::Connection> /*connection*/,
                             SimpleWeb::CaseInsensitiveMultimap & /*response_header*/) {
    return SimpleWeb::StatusCode::information_switching_protocols; // Upgrade to websocket
  };

  // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
  endpoint.on_error = [](std::shared_ptr<WsServer::Connection> connection,
                               const SimpleWeb::error_code &ec) {
    std::cout << "Server: Error in connection " << connection.get() << ". "
              << "Error: " << ec << ", error message: " << ec.message() << std::endl;
  };

  // Start server and receive assigned port when server is listening for requests
  std::promise<unsigned short> server_port;
  m_server_thread = std::async(std::launch::async, [this, &server_port]() {
    // Start server
    m_server.start([&server_port](unsigned short port) {
      server_port.set_value(port);
    });
  });
  std::cout << "Server listening on port " << server_port.get_future().get() << std::endl
            << std::endl;
}

WSMainServer::~WSMainServer() = default;

}  // namespace oac_tree_server

}  // namespace sup
