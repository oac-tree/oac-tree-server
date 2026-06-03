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

#include <sup/oac-tree-server/exceptions.h>

#include <map>

namespace sup
{
namespace oac_tree_server
{

WSServerProtocolFactory::WSServerProtocolFactory(unsigned short port)
  : IServerProtocolFactory{}
  , m_main_server{std::make_unique<WSMainServer>(port)}
{}

WSServerProtocolFactory::~WSServerProtocolFactory() = default;

std::string WSServerProtocolFactory::GetServerId() const
{
  // TODO: this should return the host IP + the port used for the websocket server:
  return std::to_string(m_main_server->GetPort());
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
  return std::make_unique<WSJobInfoIO>(publisher);
}

}  // namespace oac_tree_server

}  // namespace sup
