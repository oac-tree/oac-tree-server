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

#ifndef SUP_AUTO_SERVER_EPICS_INPUT_SERVER_H_
#define SUP_AUTO_SERVER_EPICS_INPUT_SERVER_H_

#include <sup/auto-server/input_protocol_server.h>

#include <sup/protocol/protocol_factory.h>
#include <sup/sequencer/user_input_reply.h>

#include <memory>

namespace sup
{
namespace auto_server
{

using sup::sequencer::UserInputReply;

/**
 * @brief EPICSInputServer is the EPICS implementation of an RPC server that handles user input.
 *
 * @note Clients using this server should ensure that only one input request is active at a time.
 * This means that WaitForReply should return before a new request can be made.
 */
class EPICSInputServer
{
public:
  explicit EPICSInputServer(const std::string& server_name);
  ~EPICSInputServer();

  // No copy or move
  EPICSInputServer(const EPICSInputServer& other) = delete;
  EPICSInputServer(EPICSInputServer&& other) = delete;
  EPICSInputServer& operator=(const EPICSInputServer& other) = delete;
  EPICSInputServer& operator=(EPICSInputServer&& other) = delete;

  /**
   * @brief Initiate a new request for user input.
   *
   * @param id Identifier of the new user input request.
   */
  void InitNewRequest(sup::dto::uint64 id);

  /**
   * @brief Wait for a client to provide user input or for interrupt.
   *
   * @param id Unique index that identifies a specific request for user input.
   *
   * @return Boolean indicating success of retrieving a value (true) or interrupted (false) and the
   * provided AnyValue (if true).
   */
  std::pair<bool, UserInputReply> WaitForReply(sup::dto::uint64 id);

  /**
   * @brief Interrupt an ongoing user input request.
   *
   * @param id Identifier of the user input request.
   */
  void Interrupt(sup::dto::uint64 id);

private:
  InputProtocolServer m_protocol_server;
  std::unique_ptr<sup::protocol::RPCServerInterface> m_server_stack;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_INPUT_SERVER_H_
