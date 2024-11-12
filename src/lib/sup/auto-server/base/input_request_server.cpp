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

#include <sup/auto-server/input_request_server.h>

#include <chrono>
#include <cmath>

namespace
{
bool IsValid(const sup::sequencer::UserInputReply& reply);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{

using sup::sequencer::kInvalidUserInputReply;

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

}  // namespace auto_server

}  // namespace sup

namespace
{
bool IsValid(const sup::sequencer::UserInputReply& reply)
{
  return reply.m_request_type != sup::sequencer::InputRequestType::kInvalid;
}
}  // unnamed namespace
