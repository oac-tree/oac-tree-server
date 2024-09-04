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

#include <sup/auto-server/input_protocol_client.h>

#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{

InputProtocolClient::InputProtocolClient(sup::protocol::Protocol& protocol)
  : m_protocol{protocol}
{}

InputProtocolClient::~InputProtocolClient() = default;

bool InputProtocolClient::SetClientReply(sup::dto::uint64 req_idx, const sup::dto::AnyValue& reply)
{
  sup::dto::AnyValue input = {{
    { kUserRequestIndexFieldName, sup::dto::AnyValue{req_idx} },
    { kUserReplyValueFieldName, reply }
  }};
  sup::dto::AnyValue output;
  auto result = m_protocol.Invoke(input, output);
  if (result != sup::protocol::Success)
  {
    return false;
  }
  return true;
}

}  // namespace auto_server

}  // namespace sup
