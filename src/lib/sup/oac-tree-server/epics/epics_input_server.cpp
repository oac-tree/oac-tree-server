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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "epics_input_server.h"

#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/pv_access_rpc_server.h>

namespace sup
{
namespace auto_server
{
EPICSInputServer::EPICSInputServer(const std::string& server_name)
  : m_protocol_server{}
  , m_server_stack{sup::epics::CreateEPICSRPCServerStack(
      m_protocol_server, sup::epics::GetDefaultRPCServerConfig(server_name))}
{}

EPICSInputServer::~EPICSInputServer() = default;

void EPICSInputServer::InitNewRequest(sup::dto::uint64 id)
{
  return m_protocol_server.InitNewRequest(id);
}

std::pair<bool, UserInputReply> EPICSInputServer::WaitForReply(sup::dto::uint64 id)
{
  return m_protocol_server.WaitForReply(id);
}

void EPICSInputServer::Interrupt(sup::dto::uint64 id)
{
  m_protocol_server.Interrupt(id);
}

}  // namespace auto_server

}  // namespace sup

