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

#include "epics_client_job_manager.h"
#include "epics_server_protocol_factory.h"

#include <sup/oac-tree-server/epics_utils.h>

#include <sup/oac-tree-server/epics/epics_anyvalue_manager_registry.h>

namespace sup
{
namespace oac_tree_server
{
namespace utils
{

std::unique_ptr<IServerProtocolFactory> CreateEPICSServerProtocolFactory(
  const std::string& service_name, sup::dto::uint32 n_managers)
{
  return std::make_unique<EPICSServerProtocolFactory>(service_name, n_managers);
}

std::unique_ptr<IClientJobManager> CreateEPICSClientProtocolManager(
  const std::string& service_name)
{
  return std::make_unique<EPICSClientJobManager>(service_name);
}

}  // namespace utils

}  // namespace oac_tree_server

}  // namespace sup
