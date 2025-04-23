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

#include <sup/oac-tree-server/input_request_server.h>

namespace
{
bool IsValid(const sup::oac_tree::UserInputReply& reply);
}  // unnamed namespace

namespace sup
{
namespace oac_tree_server
{

using sup::oac_tree::kInvalidUserInputReply;

InputRequestServer::InputRequestServer()
  : m_request_id{0}
  , m_reply{kInvalidUserInputReply}
  , m_interrupt{false}
  , m_mtx{}
  , m_cv{}
{}

InputRequestServer::~InputRequestServer() = default;

void InputRequestServer::InitNewRequest(sup::dto::uint64 id)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  m_request_id = id;
  m_interrupt = false;
  m_reply = kInvalidUserInputReply;
}

bool InputRequestServer::SetClientReply(sup::dto::uint64 id, const UserInputReply& reply)
{
  // Ignore invalid replies
  if (id == 0 || !IsValid(reply))
  {
    return false;
  }
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    // Refuse to set reply if index doesn't match or reply was already set:
    if (id != m_request_id || IsValid(m_reply))
    {
      return false;
    }
    m_reply = reply;
  }
  m_cv.notify_one();
  return true;
}

std::pair<bool, UserInputReply> InputRequestServer::WaitForReply(sup::dto::uint64 id)
{
  if (id == 0)
  {
    return { false, kInvalidUserInputReply };
  }
  auto pred = [this, id]() {
    auto reply_received = (IsValid(m_reply) && id == m_request_id);
    return reply_received || m_interrupt;
  };
  std::unique_lock<std::mutex> lk{m_mtx};
  m_cv.wait(lk, pred);
  if (m_interrupt)
  {
    return { false, kInvalidUserInputReply };
  }
  return { true, m_reply };
}

void InputRequestServer::Interrupt(sup::dto::uint64 id)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    if (m_request_id != id)
    {
      return;
    }
    m_interrupt = true;
  }
  m_cv.notify_one();
}

}  // namespace oac_tree_server

}  // namespace sup

namespace
{
bool IsValid(const sup::oac_tree::UserInputReply& reply)
{
  return reply.m_request_type != sup::oac_tree::InputRequestType::kInvalid;
}
}  // unnamed namespace
