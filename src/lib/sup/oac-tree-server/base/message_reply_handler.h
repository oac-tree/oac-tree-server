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
 * Copyright (c) : 2010-2026 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_OAC_TREE_SERVER_MESSAGE_REPLY_HANDLER_H_
#define SUP_OAC_TREE_SERVER_MESSAGE_REPLY_HANDLER_H_

#include "index_generator.h"

#include <sup/dto/basic_scalar_types.h>

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief Class that uses unique ids to track message replies.
 */
class MessageReplyHandler
{
public:
  class IdGuard
  {
  public:
    IdGuard(MessageReplyHandler& handler, sup::dto::uint64 id);
    ~IdGuard();

    IdGuard(const IdGuard& other) = delete;
    IdGuard& operator=(const IdGuard& other) = delete;

    sup::dto::uint64 GetId() const;

  private:
    MessageReplyHandler& m_handler;
    sup::dto::uint64 m_id;
  };

  MessageReplyHandler();
  ~MessageReplyHandler();

  IdGuard CreateNewMessageId();

  void HandleReply(sup::dto::uint64 message_id, const std::string& reply);

  std::optional<std::string> WaitForReply(sup::dto::uint64 message_id,
                                          std::chrono::milliseconds timeout);

  void CancelMessage(sup::dto::uint64 message_id);

private:
  IndexGenerator m_idx_generator;
  mutable std::mutex m_mtx;
  std::condition_variable m_cv;
  std::unordered_map<sup::dto::uint64, std::optional<std::string>> m_replies;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_MESSAGE_REPLY_HANDLER_H_
