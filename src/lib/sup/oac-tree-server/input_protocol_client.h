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

#ifndef SUP_OAC_TREE_SERVER_INPUT_PROTOCOL_CLIENT_H_
#define SUP_OAC_TREE_SERVER_INPUT_PROTOCOL_CLIENT_H_

#include <sup/dto/anyvalue.h>
#include <sup/protocol/protocol.h>
#include <sup/oac-tree/user_input_reply.h>

namespace sup
{
namespace oac_tree_server
{
using sup::oac_tree::UserInputReply;

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

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_INPUT_PROTOCOL_CLIENT_H_
