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
 * Copyright (c) : 2010-2025 ITER Organization,
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

#include "epics_io_client.h"

#include <sup/oac-tree-server/automation_client_stack.h>
#include <sup/oac-tree-server/epics/epics_anyvalue_manager_registry.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>
#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/pv_access_rpc_client.h>

namespace sup
{
namespace oac_tree_server
{
namespace utils
{

std::unique_ptr<IAnyValueIO> CreateEPICSIOClient(IAnyValueManager& av_mgr)
{
  return std::make_unique<EPICSIOClient>(av_mgr);
}

std::unique_ptr<IJobManager> CreateEPICSJobManager(const std::string& server_name)
{
  auto info_config = sup::epics::GetDefaultRPCClientConfig(server_name);
  auto info_protocol = sup::epics::CreateEPICSRPCClientStack(info_config);
  auto control_server_name = GetControlServerName(server_name);
  auto control_config = sup::epics::GetDefaultRPCClientConfig(control_server_name);
  auto control_protocol = sup::epics::CreateEPICSRPCClientStack(control_config);
  auto result = std::make_unique<AutomationClientStack>(std::move(info_protocol),
                                                        std::move(control_protocol));
  return result;
}

std::unique_ptr<IAnyValueManagerRegistry> CreateEPICSAnyValueManagerRegistry(
    sup::dto::uint32 n_managers)
{
  auto result = std::make_unique<EPICSAnyValueManagerRegistry>(n_managers);
  return result;
}

}  // namespace utils

}  // namespace oac_tree_server

}  // namespace sup
