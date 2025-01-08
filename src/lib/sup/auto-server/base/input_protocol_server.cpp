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

#include <sup/auto-server/input_protocol_server.h>

#include <sup/auto-server/input_reply_helper.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/protocol/function_protocol_extract.h>
#include <sup/protocol/protocol_rpc.h>

namespace sup
{
namespace auto_server
{

InputProtocolServer::InputProtocolServer()
  : m_request_server{}
{}

InputProtocolServer::~InputProtocolServer() = default;

void InputProtocolServer::InitNewRequest(sup::dto::uint64 id)
{
  return m_request_server.InitNewRequest(id);
}

std::pair<bool, UserInputReply> InputProtocolServer::WaitForReply(sup::dto::uint64 id)
{
  return m_request_server.WaitForReply(id);
}

void InputProtocolServer::Interrupt(sup::dto::uint64 id)
{
  m_request_server.Interrupt(id);
}

sup::protocol::ProtocolResult InputProtocolServer::Invoke(const sup::dto::AnyValue& input,
                                                          sup::dto::AnyValue& output)
{
  return sup::protocol::CallFunctionProtocol(*this, FunctionMap(), input, output, NotSupported);
}

sup::protocol::ProtocolResult InputProtocolServer::Service(const sup::dto::AnyValue& input,
                                                           sup::dto::AnyValue& output)
{
  if (sup::protocol::utils::IsApplicationProtocolRequestPayload(input))
  {
    return sup::protocol::utils::HandleApplicationProtocolInfo(
      output, kAutomationInputRequestServerType, kAutomationInputRequestServerVersion);
  }
  return NotSupported;
}

const sup::protocol::ProtocolMemberFunctionMap<InputProtocolServer>&
InputProtocolServer::FunctionMap()
{
  static sup::protocol::ProtocolMemberFunctionMap<InputProtocolServer> f_map = {
    { KSetReplyFunctionName, &InputProtocolServer::SetClientReply }
  };
  return f_map;
}

sup::protocol::ProtocolResult InputProtocolServer::SetClientReply(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  (void)output;
  sup::dto::AnyValue user_reply_av;
  if (!sup::protocol::FunctionProtocolExtract(user_reply_av, input, kUserReplyValueFieldName))
  {
    return sup::protocol::ServerProtocolDecodingError;
  };
  auto decoded = DecodeInputReply(user_reply_av);
  if (!std::get<0>(decoded))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  auto id = std::get<1>(decoded);
  auto user_reply = std::get<2>(decoded);
  auto result = m_request_server.SetClientReply(id, user_reply);
  if (!result)
  {
    return ClientReplyRefused;
  }
  return sup::protocol::Success;
}

}  // namespace auto_server

}  // namespace sup
