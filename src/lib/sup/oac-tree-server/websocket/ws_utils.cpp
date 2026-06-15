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

#include "ws_client_job_manager.h"
#include "ws_server_protocol_factory.h"

#include <sup/oac-tree-server/ws_utils.h>

namespace
{
constexpr std::chrono::milliseconds kDefaultReplyTimeout{5000};
}

namespace sup
{
namespace oac_tree_server
{
namespace utils
{

std::unique_ptr<IServerProtocolFactory> CreateWSServerProtocolFactory(unsigned short port)
{
  return std::make_unique<WSServerProtocolFactory>(port);
}

std::unique_ptr<IClientJobManager> CreateWSClientJobManager(
  const std::string& host_name, unsigned short port)
{
  return std::make_unique<WSClientJobManager>(host_name, port, kDefaultReplyTimeout);
}

std::unique_ptr<IClientJobManager> CreateWSClientJobManager(
  const std::string& host_name, unsigned short port, std::chrono::milliseconds reply_timeout)
{
  return std::make_unique<WSClientJobManager>(host_name, port, reply_timeout);
}

}  // namespace utils

}  // namespace oac_tree_server

}  // namespace sup
