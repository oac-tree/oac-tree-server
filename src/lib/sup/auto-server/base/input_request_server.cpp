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
const double kMaxTimeoutSeconds = 9.2e9; // More than 290 years. This should be enough...
const sup::dto::int64 kMaxTimeoutNanoseconds = 9200000000000000000LL;

// Increments the current index and avoids returning zero after wrap around.
sup::dto::uint64 GetNextRequestIndex(sup::dto::uint64 current_idx);

// Convert a timeout in seconds (floating point) to nanoseconds (integer).
sup::dto::int64 ConvertToTimeoutNanoseconds(double timeout_sec);

}  // unnamed namespace

namespace sup
{
namespace auto_server
{

InputRequestServer::InputRequestServer()
  : m_request_idx{0}
  , m_reply{}
  , m_mtx{}
  , m_cv{}
{}

InputRequestServer::~InputRequestServer() = default;

bool InputRequestServer::SetClientReply(sup::dto::uint64 req_idx, const sup::dto::AnyValue& reply)
{
  // Do not allow setting an empty reply:
  if (sup::dto::IsEmptyValue(reply))
  {
    return false;
  }
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    if (req_idx != m_request_idx)
    {
      return false;
    }
    m_reply = reply;
  }
  m_cv.notify_one();
  return true;
}

sup::dto::uint64 InputRequestServer::InitNewRequest()
{
  std::lock_guard<std::mutex> lk{m_mtx};
  m_request_idx = GetNextRequestIndex(m_request_idx);
  m_reply = sup::dto::AnyValue{};
  return m_request_idx;
}

std::pair<bool, sup::dto::AnyValue> InputRequestServer::WaitForReply(sup::dto::uint64 req_idx,
                                                                     double timeout_sec)
{
  auto timeout_ns = ConvertToTimeoutNanoseconds(timeout_sec);
  auto pred = [this, req_idx]() {
    return !sup::dto::IsEmptyValue(m_reply) && req_idx == m_request_idx;
  };
  std::unique_lock<std::mutex> lk{m_mtx};
  if (m_cv.wait_for(lk, std::chrono::nanoseconds(timeout_ns), pred))
  {
    return { true, m_reply };
  }
  return { false, {} };
}

}  // namespace auto_server

}  // namespace sup

namespace
{
sup::dto::uint64 GetNextRequestIndex(sup::dto::uint64 current_idx)
{
  ++current_idx;
  if (current_idx > 0)
  {
    return current_idx;
  }
  return 1u;
}

sup::dto::int64 ConvertToTimeoutNanoseconds(double timeout_sec)
{
  if (timeout_sec < 0.0)
  {
    return 0;
  }
  if (timeout_sec > kMaxTimeoutSeconds)
  {
    return kMaxTimeoutNanoseconds;
  }
  return std::lround(timeout_sec * 1e9);
}

}  // unnamed namespace
