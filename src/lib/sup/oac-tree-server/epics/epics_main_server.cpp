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

#include "epics_main_server.h"

#include <sup/oac-tree-server/control_protocol_server.h>
#include <sup/oac-tree-server/info_protocol_server.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/epics/epics_protocol_factory.h>

namespace sup
{
namespace oac_tree_server
{

EPICSMainServer::EPICSMainServer(const std::string& service_name,
                                 IJobManager& job_manager)
  : IMainServer{}
  , m_info_server_stack{}
  , m_control_server_stack{}
{
  // Instantiate RPC server for obtaining job information
  auto info_server_protocol = std::make_unique<InfoProtocolServer>(job_manager);
  sup::epics::PvAccessRPCServerConfig info_server_config{service_name};
  m_info_server_stack = sup::epics::CreateEPICSRPCServerStack(
    info_server_config, sup::protocol::ProtocolRPCServerConfig{}, std::move(info_server_protocol));
  // Instantiate RPC server for controlling jobs
  auto control_server_protocol = std::make_unique<ControlProtocolServer>(job_manager);
  auto control_service_name = GetControlServerName(service_name);
  sup::epics::PvAccessRPCServerConfig control_server_config{control_service_name};
  m_control_server_stack = sup::epics::CreateEPICSRPCServerStack(
    control_server_config, sup::protocol::ProtocolRPCServerConfig{},
    std::move(control_server_protocol));
}

EPICSMainServer::~EPICSMainServer() = default;

}  // namespace oac_tree_server

}  // namespace sup
