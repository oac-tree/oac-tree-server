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

#include <sup/oac-tree-server/input_protocol_client.h>

#include <sup/oac-tree-server/input_reply_helper.h>
#include <sup/oac-tree-server/sup_auto_protocol.h>

#include <sup/protocol/function_protocol.h>
#include <sup/protocol/function_protocol_pack.h>

namespace sup
{
namespace auto_server
{

InputProtocolClient::InputProtocolClient(sup::protocol::Protocol& protocol)
  : m_protocol{protocol}
{}

InputProtocolClient::~InputProtocolClient() = default;

bool InputProtocolClient::SetClientReply(sup::dto::uint64 id, const UserInputReply& reply)
{
  auto input = sup::protocol::FunctionProtocolInput(KSetReplyFunctionName);
  auto encoded = EncodeInputReply(id, reply);
  sup::protocol::FunctionProtocolPack(input, kUserReplyValueFieldName, encoded);
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
