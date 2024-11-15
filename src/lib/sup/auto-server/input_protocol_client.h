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

#ifndef SUP_AUTO_SERVER_INPUT_PROTOCOL_CLIENT_H_
#define SUP_AUTO_SERVER_INPUT_PROTOCOL_CLIENT_H_

#include <sup/dto/anyvalue.h>
#include <sup/protocol/protocol.h>
#include <sup/sequencer/user_input_reply.h>

namespace sup
{
namespace auto_server
{
using sup::sequencer::UserInputReply;

/**
 * @brief InputProtocolClient .
 */
class InputProtocolClient
{
public:
  InputProtocolClient(sup::protocol::Protocol& protocol);
  ~InputProtocolClient();

  // No copy or move
  InputProtocolClient(const InputProtocolClient& other) = delete;
  InputProtocolClient(InputProtocolClient&& other) = delete;
  InputProtocolClient& operator=(const InputProtocolClient& other) = delete;
  InputProtocolClient& operator=(InputProtocolClient&& other) = delete;

  bool SetClientReply(sup::dto::uint64 id, const UserInputReply& reply);
private:
  sup::protocol::Protocol& m_protocol;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INPUT_PROTOCOL_CLIENT_H_
