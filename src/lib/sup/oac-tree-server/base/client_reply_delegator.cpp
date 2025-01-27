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

#include <sup/oac-tree-server/client_reply_delegator.h>

#include <algorithm>

namespace sup
{
namespace oac_tree_server
{

ClientReplyDelegator::ClientReplyDelegator(ReplyFunction reply_func, InterruptFunction interrupt_func)
  : m_reply_func{reply_func}
  , m_interrupt_func{interrupt_func}
  , m_active_id{0}
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

void ClientReplyDelegator::InterruptAll()
{
  // Empty whole queue (there can be no input request valid anymore) and call interrupt for
  // possibly active id.
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_reply_queue.clear();
    if (m_active_id != 0)
    {
      m_interrupt_func(m_active_id);
    }
  }
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
      m_active_id = reply_info.id;
      lk.unlock();
      m_reply_func(reply_info.id, reply_info.reply);
      lk.lock();
      m_active_id = 0;
    }
  }
}

void ClientReplyDelegator::Halt()
{
  m_halt.store(true);
  m_cv.notify_one();
}

}  // namespace oac_tree_server

}  // namespace sup
