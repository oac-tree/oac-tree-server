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

#include "epics_input_server.h"

#include <sup/oac-tree-server/input_protocol_server.h>

#include <sup/epics/epics_protocol_factory.h>

namespace sup
{
namespace oac_tree_server
{
EPICSInputServer::EPICSInputServer(const std::string& server_name)
  : m_request_server{}
  , m_server_stack{sup::epics::CreateEPICSRPCServerStack(
      sup::epics::GetDefaultRPCServerConfig(server_name), sup::protocol::ProtocolRPCServerConfig{},
      std::make_unique<InputProtocolServer>(m_request_server))}
{}

EPICSInputServer::~EPICSInputServer() = default;

void EPICSInputServer::InitNewRequest(sup::dto::uint64 id)
{
  return m_request_server.InitNewRequest(id);
}

std::pair<bool, UserInputReply> EPICSInputServer::WaitForReply(sup::dto::uint64 id)
{
  return m_request_server.WaitForReply(id);
}

void EPICSInputServer::Interrupt(sup::dto::uint64 id)
{
  m_request_server.Interrupt(id);
}

}  // namespace oac_tree_server

}  // namespace sup

