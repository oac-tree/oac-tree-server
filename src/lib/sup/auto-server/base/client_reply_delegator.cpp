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

#include <sup/auto-server/client_reply_delegator.h>

namespace sup
{
namespace auto_server
{

ClientReplyDelegator::ClientReplyDelegator(ReplyFunction reply_func)
  : m_reply_func{reply_func}
  , m_cv{}
  , m_mtx{}
  , m_halt{false}
  , m_reply_queue{}
  , m_delegatee{&ClientReplyDelegator::HandleClientReply, this}
{}

ClientReplyDelegator::~ClientReplyDelegator()
{
  Halt();
  m_delegatee.join();
}

void ClientReplyDelegator::QueueReply(sup::dto::uint64 id, const UserInputReply& reply)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_reply_queue.push_back({id, reply});
  }
  m_cv.notify_one();
}

void ClientReplyDelegator::HandleClientReply()
{
  auto pred = [this]() {
    return !m_reply_queue.empty() || m_halt.load();
  };
  while (m_halt.load() == false)
  {
    std::unique_lock<std::mutex> lk{m_mtx};
    m_cv.wait(lk, pred);
    if (!m_reply_queue.empty())
    {
      auto reply_info = m_reply_queue.front();
      m_reply_queue.pop_front();
      lk.unlock();
      m_reply_func(reply_info.id, reply_info.reply);
    }
  }
}

void ClientReplyDelegator::Halt()
{
  m_halt.store(true);
  m_cv.notify_one();
}

}  // namespace auto_server

}  // namespace sup
