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

#include "ws_main_server.h"

#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <map>

namespace sup
{
namespace oac_tree_server
{

struct WSServerProtocolFactory::Endpoints
{
public:
  using Endpoint = WSMainServer::WsServer::Endpoint;

  Endpoints() : m_endpoint_map{} {}

  std::map<std::string, Endpoint> m_endpoint_map;
};

WSServerProtocolFactory::WSServerProtocolFactory(unsigned short port)
  : IServerProtocolFactory{}
  , m_port{port}
  , m_endpoints{std::make_unique<Endpoints>()}
{}

WSServerProtocolFactory::~WSServerProtocolFactory() = default;

std::string WSServerProtocolFactory::GetServerId() const
{
  // TODO: this should return the host IP + the port used for the websocket server:
  return "WS:";
}

std::unique_ptr<IMainServer> WSServerProtocolFactory::CreateMainServer(IJobManager& job_manager)
{
  return std::make_unique<WSMainServer>(m_port, job_manager);
}

std::unique_ptr<sup::oac_tree::IJobInfoIO> WSServerProtocolFactory::CreateJobInfoIO(
  sup::dto::uint32 job_idx, sup::dto::uint32 n_vars)
{
  // TODO: create a WsServer::Endpoint for this job (with regex and currently no special handlers)
  // Then create a WSJobInfoIO that has a reference to this endpoint and can use it to send updates
  // Since it uses its connections to send updates, it doesn't need to be deactivated while the
  // server is not yet running.
  (void)job_idx;
  (void)n_vars;
  return {};
}

}  // namespace oac_tree_server

}  // namespace sup
