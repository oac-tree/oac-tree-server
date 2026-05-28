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

namespace sup
{
namespace oac_tree_server
{

WSServerProtocolFactory::WSServerProtocolFactory(unsigned short port)
  : IServerProtocolFactory{}
  , m_port{port}
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
  (void)job_idx;
  (void)n_vars;
  return {};
}

}  // namespace oac_tree_server

}  // namespace sup
