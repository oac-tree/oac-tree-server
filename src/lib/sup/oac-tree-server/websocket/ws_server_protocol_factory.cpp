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

#include "ws_server_protocol_factory.h"

#include "ws_job_info_io.h"
#include "ws_main_server.h"
#include "ws_message.h"
#include "ws_message_handler.h"

#include <sup/oac-tree-server/base/input_request_server.h>

#include <sup/oac-tree-server/exceptions.h>

#include <map>
#include <memory>

namespace sup
{
namespace oac_tree_server
{

WSServerProtocolFactory::WSServerProtocolFactory(unsigned short port,
                                                 std::chrono::milliseconds resend_interval)
  : IServerProtocolFactory{}
  , m_main_server{std::make_unique<WSMainServer>(port)}
  , m_resend_interval{resend_interval}
{
  if (m_resend_interval.count() <= 0)
  {
    throw InvalidOperationException("WSServerProtocolFactory requires a positive resend interval");
  }
}

WSServerProtocolFactory::~WSServerProtocolFactory() = default;

std::string WSServerProtocolFactory::GetServerId() const
{
  // WebSocket clients connect using the host:port they were configured with and do not rely on this
  // value to locate the job endpoints, so reporting localhost together with the port is sufficient.
  return CreateServerPrefix("localhost", m_main_server->GetPort());
}

std::unique_ptr<IMainServer> WSServerProtocolFactory::CreateMainServer(IJobManager& job_manager)
{
  if (!m_main_server)
  {
    throw InvalidOperationException("Main server was already created");
  }
  m_main_server->AddJobManagerEndpoint(job_manager);
  m_main_server->Start();
  return std::move(m_main_server);
}

std::unique_ptr<sup::oac_tree::IJobInfoIO> WSServerProtocolFactory::CreateJobInfoIO(
  sup::dto::uint32 job_idx, sup::dto::uint32 n_vars)
{
  (void)n_vars;
  if (!m_main_server)
  {
    throw InvalidOperationException("Main server was already created");
  }
  auto& endpoint = m_main_server->AddJobEndpoint(job_idx);
  auto publisher = [&endpoint](std::string message) {
    // Send the message to all clients connected to this job's endpoint
    // Before starting the server, there are no clients connected, so this will just do nothing.
    for (auto& connection : endpoint.get_connections())
    {
      connection->send(message);
    }
  };
  // The request server is shared between the WSJobInfoIO (which waits for replies on the job
  // execution thread) and the endpoint's message handler (which delivers client replies from the
  // server's io_service thread), so it must outlive both.
  auto request_server = std::make_shared<InputRequestServer>();
  endpoint.on_message =
    [request_server](std::shared_ptr<WSMainServer::WsServer::Connection> /*connection*/,
                     std::shared_ptr<WSMainServer::WsServer::InMessage> in_message) {
      HandleUserInputReply(in_message->string(), *request_server);
    };
  return std::make_unique<WSJobInfoIO>(publisher, request_server, m_resend_interval);
}

}  // namespace oac_tree_server

}  // namespace sup
