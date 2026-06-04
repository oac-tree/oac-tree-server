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

#include "ws_message.h"

#include <sup/oac-tree-server/exceptions.h>
#include <sup/oac-tree-server/websocket/ws_message_handler.h>

namespace sup
{
namespace oac_tree_server
{
namespace
{
std::string CreateEndpointRegex(const std::string& endpoint);
}  // unnamed namespace

WSMainServer::WSMainServer(unsigned short port)
  : IMainServer{}
  , m_server{}
  , m_server_thread{}
{
  m_server.config.port = port;
}

WSMainServer::~WSMainServer() = default;

unsigned short WSMainServer::GetPort() const
{
  return m_server.config.port;
}

WSMainServer::WsServer::Endpoint& WSMainServer::AddJobEndpoint(sup::dto::uint32 job_idx)
{
  auto job_regex = GetJobRegex(job_idx);
  auto &endpoint = m_server.endpoint[job_regex];

  // Can modify handshake response headers here if needed
  endpoint.on_handshake = [](std::shared_ptr<WsServer::Connection> /*connection*/,
                             SimpleWeb::CaseInsensitiveMultimap & /*response_header*/) {
    return SimpleWeb::StatusCode::information_switching_protocols; // Upgrade to websocket
  };

  // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
  endpoint.on_error = [](std::shared_ptr<WsServer::Connection> connection,
                               const SimpleWeb::error_code &ec) {
    std::cerr << "Server: Error in connection " << connection.get() << ". "
              << "Error: " << ec << ", error message: " << ec.message() << std::endl;
  };
  return endpoint;
}

void WSMainServer::AddJobManagerEndpoint(IJobManager& job_manager)
{
  auto job_manager_regex = GetJobManagerRegex();
  auto &endpoint = m_server.endpoint[job_manager_regex];

  endpoint.on_message = [&job_manager](std::shared_ptr<WsServer::Connection> connection,
                               std::shared_ptr<WsServer::InMessage> in_message) {
    auto out_message = HandleJobManagerMessage(in_message->string(), job_manager);
    if (!out_message.empty())
    {
      connection->send(out_message);
    }
  };

  // Can modify handshake response headers here if needed
  endpoint.on_handshake = [](std::shared_ptr<WsServer::Connection> /*connection*/,
                             SimpleWeb::CaseInsensitiveMultimap & /*response_header*/) {
    return SimpleWeb::StatusCode::information_switching_protocols; // Upgrade to websocket
  };

  // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
  endpoint.on_error = [](std::shared_ptr<WsServer::Connection> connection,
                               const SimpleWeb::error_code &ec) {
    std::cerr << "Server: Error in connection " << connection.get() << ". "
              << "Error: " << ec << ", error message: " << ec.message() << std::endl;
  };
}

void WSMainServer::Start()
{
  if (m_server_thread.valid())
  {
    throw InvalidOperationException("Server is already running");
  }
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

std::string GetJobManagerRegex()
{
  return CreateEndpointRegex(endpoints::kJobManagerEndpoint);
}

std::string GetJobRegex(sup::dto::uint32 job_idx)
{
  return CreateEndpointRegex(std::string(endpoints::kJobEndpointPrefix) + std::to_string(job_idx));
}

namespace
{
std::string CreateEndpointRegex(const std::string& endpoint)
{
  return "^" + endpoint + "/?$";
}
}  // unnamed namespace

}  // namespace oac_tree_server

}  // namespace sup
