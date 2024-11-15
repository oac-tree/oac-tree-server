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

#ifndef SUP_AUTO_SERVER_EPICS_INPUT_CLIENT_H_
#define SUP_AUTO_SERVER_EPICS_INPUT_CLIENT_H_

#include <sup/auto-server/input_protocol_client.h>

#include <sup/protocol/protocol_factory.h>

#include <memory>

namespace sup
{
namespace auto_server
{

/**
 * @brief EPICSInputClient is the EPICS implementation of an RPC client that responds to user input
 * requests.
 */
class EPICSInputClient
{
public:
  explicit EPICSInputClient(const std::string& server_name);
  ~EPICSInputClient();

  // No copy or move
  EPICSInputClient(const EPICSInputClient& other) = delete;
  EPICSInputClient(EPICSInputClient&& other) = delete;
  EPICSInputClient& operator=(const EPICSInputClient& other) = delete;
  EPICSInputClient& operator=(EPICSInputClient&& other) = delete;

  bool SetClientReply(sup::dto::uint64 id, const UserInputReply& reply);
private:
  std::unique_ptr<sup::protocol::Protocol> m_client_stack;
  InputProtocolClient m_protocol_client;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_INPUT_CLIENT_H_
