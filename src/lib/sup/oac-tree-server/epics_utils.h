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

#ifndef SUP_OAC_TREE_SERVER_EPICS_UTILS_H_
#define SUP_OAC_TREE_SERVER_EPICS_UTILS_H_

#include <sup/oac-tree-server/i_server_protocol_factory.h>
#include <sup/oac-tree-server/i_client_protocol_manager.h>

#include <memory>
#include <string>

namespace sup
{
namespace oac_tree_server
{
namespace utils
{

// TODO: remove this (or move to client manager)
std::unique_ptr<IServerProtocolFactory> CreateEPICSServerProtocolFactory(
  const std::string& service_name, sup::dto::uint32 n_managers);

std::unique_ptr<IClientProtocolManager> CreateEPICSClientProtocolManager(
  const std::string& service_name);

}  // namespace utils

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_EPICS_UTILS_H_
