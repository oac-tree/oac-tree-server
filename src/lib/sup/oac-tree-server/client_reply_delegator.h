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
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_OAC_TREE_SERVER_CLIENT_REPLY_DELEGATOR_H_
#define SUP_OAC_TREE_SERVER_CLIENT_REPLY_DELEGATOR_H_

#include <sup/oac-tree/user_input_reply.h>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

namespace sup
{
namespace oac_tree_server
{
using sup::oac_tree::UserInputReply;

/**
 * @brief ClientReplyDelegator delegates a call to reply to user input to a separate thread. This
 * is required to avoid deadlocks in case of network implementations that are not lock-free, e.g.
 * EPICS.
 */
class ClientReplyDelegator
{
public:
  using ReplyFunction = std::function<void(sup::dto::uint64, const UserInputReply&)>;
  using InterruptFunction = std::function<void(sup::dto::uint64)>;
  explicit ClientReplyDelegator(ReplyFunction reply_func, InterruptFunction interrupt_func);
  ~ClientReplyDelegator();

  // No copy or move
  ClientReplyDelegator(const ClientReplyDelegator& other) = delete;
  ClientReplyDelegator(ClientReplyDelegator&& other) = delete;
  ClientReplyDelegator& operator=(const ClientReplyDelegator& other) = delete;
  ClientReplyDelegator& operator=(ClientReplyDelegator&& other) = delete;

  void QueueReply(sup::dto::uint64 id, const UserInputReply& reply);

  void InterruptAll();

private:
  void HandleClientReply();
  void Halt();
  struct ReplyInfo
  {
    sup::dto::uint64 id;
    UserInputReply reply;
  };
  ReplyFunction m_reply_func;
  InterruptFunction m_interrupt_func;
  sup::dto::uint64 m_active_id;
  std::condition_variable m_cv;
  std::mutex m_mtx;
  std::atomic_bool m_halt;
  std::deque<ReplyInfo> m_reply_queue;
  std::thread m_delegatee;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_CLIENT_REPLY_DELEGATOR_H_
