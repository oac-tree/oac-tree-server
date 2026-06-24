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

#include "message_reply_handler.h"

namespace sup
{
namespace oac_tree_server
{

MessageReplyHandler::IdGuard::IdGuard(MessageReplyHandler& handler, sup::dto::uint64 id)
  : m_handler(handler), m_id(id) {}

MessageReplyHandler::IdGuard::~IdGuard()
{
  m_handler.CancelMessage(m_id);
}

sup::dto::uint64 MessageReplyHandler::IdGuard::GetId() const
{
  return m_id;
}

MessageReplyHandler::MessageReplyHandler()
  : m_idx_generator{}
  , m_mtx{}
  , m_cv{}
  , m_replies{}
{}

MessageReplyHandler::~MessageReplyHandler() = default;

MessageReplyHandler::IdGuard MessageReplyHandler::CreateNewMessageId()
{
  sup::dto::uint64 id = m_idx_generator.NewIndex();
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_replies[id] = std::nullopt;
  }
  return IdGuard(*this, id);
}

void MessageReplyHandler::HandleReply(sup::dto::uint64 message_id, const std::string& reply)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    auto iter = m_replies.find(message_id);
    if (iter == m_replies.end())
    {
      return;
    }
    iter->second = reply;
  }
  m_cv.notify_all();
}

std::optional<std::string> MessageReplyHandler::WaitForReply(sup::dto::uint64 message_id,
                                                             std::chrono::milliseconds timeout)
{
  {
    std::unique_lock<std::mutex> lk{m_mtx};
    auto iter = m_replies.find(message_id);
    if (iter == m_replies.end())
    {
      return std::nullopt;
    }
    if (m_cv.wait_for(lk, timeout, [&]{ return iter->second.has_value(); }))
    {
      std::optional<std::string> reply = std::move(iter->second);
      (void)m_replies.erase(iter);
      return reply;
    }
  }
  return std::nullopt;
}

void MessageReplyHandler::CancelMessage(sup::dto::uint64 message_id)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  (void)m_replies.erase(message_id);
}

}  // namespace oac_tree_server

}  // namespace sup
