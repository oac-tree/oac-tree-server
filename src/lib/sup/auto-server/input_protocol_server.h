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

#ifndef SUP_AUTO_SERVER_INPUT_PROTOCOL_SERVER_H_
#define SUP_AUTO_SERVER_INPUT_PROTOCOL_SERVER_H_

#include <sup/auto-server/input_request_server.h>

#include <sup/protocol/function_protocol.h>
#include <sup/protocol/protocol.h>

namespace sup
{
namespace auto_server
{

/**
 * @brief InputProtocolServer exposes the sup protocol for clients providing user input. It handles
 * only one input request at a time, so clients of this code should make sure only one request is
 * active at a time.
 */
class InputProtocolServer : public sup::protocol::Protocol
{
public:
  InputProtocolServer();
  ~InputProtocolServer();

  // No copy or move
  InputProtocolServer(const InputProtocolServer& other) = delete;
  InputProtocolServer(InputProtocolServer&& other) = delete;
  InputProtocolServer& operator=(const InputProtocolServer& other) = delete;
  InputProtocolServer& operator=(InputProtocolServer&& other) = delete;

  void InitNewRequest(sup::dto::uint64 id);
  std::pair<bool, UserInputReply> WaitForReply(sup::dto::uint64 id);
  void Interrupt(sup::dto::uint64 id);

  sup::protocol::ProtocolResult Invoke(const sup::dto::AnyValue& input,
                                  sup::dto::AnyValue& output) override;
  sup::protocol::ProtocolResult Service(const sup::dto::AnyValue& input,
                                  sup::dto::AnyValue& output) override;
private:
  static const sup::protocol::ProtocolMemberFunctionMap<InputProtocolServer>& FunctionMap();
  InputRequestServer m_request_server;
  sup::protocol::ProtocolResult SetClientReply(const sup::dto::AnyValue& input,
                                               sup::dto::AnyValue& output);
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INPUT_PROTOCOL_SERVER_H_
