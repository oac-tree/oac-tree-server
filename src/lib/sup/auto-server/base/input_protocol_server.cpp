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

#include <sup/auto-server/input_protocol_server.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/protocol/function_protocol_extract.h>
#include <sup/protocol/protocol_rpc.h>

namespace sup
{
namespace auto_server
{

namespace status
{
enum ConfigurationStatus
{
  kNotSupported = sup::protocol::SPECIFIC_APPLICATION_ERROR_START,
  kClientReplyRefused
};
}  // namespace status

const sup::protocol::ProtocolResult NotSupported{status::kNotSupported};
const sup::protocol::ProtocolResult ClientReplyRefused{status::kClientReplyRefused};

// Input request servers will report the following type and version:
const std::string kAutomationInputRequestServerType = "SUP::AutoInputServerProtocol";
const std::string kAutomationInputRequestServerVersion = "1.0";

// Supported function names for input request servers:
const std::string KSetReplyFunctionName = "SetReply";

// Field names used for the supported functions of input request servers:
const std::string kUserRequestIndexFieldName = "index";
const std::string kUserReplyValueFieldName = "value";

InputProtocolServer::InputProtocolServer()
  : m_request_server{}
{}

InputProtocolServer::~InputProtocolServer() = default;

sup::dto::uint64 InputProtocolServer::InitNewRequest()
{
  return m_request_server.InitNewRequest();
}

std::pair<bool, sup::dto::AnyValue> InputProtocolServer::WaitForReply(sup::dto::uint64 req_idx,
                                                                      double timeout_sec)
{
  return m_request_server.WaitForReply(req_idx, timeout_sec);
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
  sup::dto::AnyValue request_idx_av;
  if (!sup::protocol::FunctionProtocolExtract(request_idx_av, input, kUserRequestIndexFieldName))
  {
    return sup::protocol::ServerProtocolDecodingError;
  };
  if (request_idx_av.GetType() != sup::dto::UnsignedInteger64Type)
  {
    return sup::protocol::ServerProtocolDecodingError;
  };
  auto request_idx = request_idx_av.As<sup::dto::uint64>();
  sup::dto::AnyValue user_reply;
  if (!sup::protocol::FunctionProtocolExtract(user_reply, input, kUserReplyValueFieldName))
  {
    return sup::protocol::ServerProtocolDecodingError;
  };
  auto result = m_request_server.SetClientReply(request_idx, user_reply);
  if (!result)
  {
    return ClientReplyRefused;
  }
  return sup::protocol::Success;
}

}  // namespace auto_server

}  // namespace sup
