/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVER
 *
 * Description   : SUP automation server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "epics_input_client.h"

#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/pv_access_rpc_client.h>

namespace sup
{
namespace auto_server
{

EPICSInputClient::EPICSInputClient(const std::string& server_name)
  : m_client_stack{sup::epics::CreateEPICSRPCClientStack(
                       sup::epics::PvAccessRPCClientConfig{server_name, 10.0})}
  , m_protocol_client{*m_client_stack}
{}

EPICSInputClient::~EPICSInputClient() = default;

bool EPICSInputClient::SetClientReply(sup::dto::uint64 id, const UserInputReply& reply)
{
  return m_protocol_client.SetClientReply(id, reply);
}

}  // namespace auto_server

}  // namespace sup
