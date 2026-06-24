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

#include "ws_client_input_delegator.h"

#include "ws_message.h"

#include <algorithm>
#include <utility>

namespace sup
{
namespace oac_tree_server
{
namespace
{
// Number of recently finished request ids kept to discard re-published requests that arrive shortly
// after a request finished. Only one request is outstanding at a time, so a handful is plenty.
constexpr std::size_t kRecentFinishedCapacity = 16;
}  // namespace

WSClientInputDelegator::WSClientInputDelegator(sup::oac_tree::IJobInfoIO& job_info_io)
  : m_job_info_io{job_info_io}
  , m_mtx{}
  , m_cv{}
  , m_queue{}
  , m_recent_finished{}
  , m_active_id{0}
  , m_active_cancelled{false}
  , m_halt{false}
  , m_worker{&WSClientInputDelegator::Run, this}
{}

WSClientInputDelegator::~WSClientInputDelegator()
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_halt = true;
    // Unblock a prompt that is currently waiting for user input, so the worker can exit.
    if (m_active_id != 0)
    {
      m_active_cancelled = true;
      m_job_info_io.Interrupt(m_active_id);
    }
  }
  m_cv.notify_all();
  if (m_worker.joinable())
  {
    m_worker.join();
  }
}

void WSClientInputDelegator::QueueUserValueRequest(sup::dto::uint64 id,
                                                   const sup::dto::AnyValue& value,
                                                   const std::string& description,
                                                   SendFunction send)
{
  RequestInfo info{};
  info.id = id;
  info.kind = RequestKind::kValue;
  info.value = value;
  info.description = description;
  info.send = std::move(send);
  Enqueue(std::move(info));
}

void WSClientInputDelegator::QueueUserChoiceRequest(sup::dto::uint64 id,
                                                    const std::vector<std::string>& options,
                                                    const sup::dto::AnyValue& metadata,
                                                    SendFunction send)
{
  RequestInfo info{};
  info.id = id;
  info.kind = RequestKind::kChoice;
  info.options = options;
  info.metadata = metadata;
  info.send = std::move(send);
  Enqueue(std::move(info));
}

void WSClientInputDelegator::Cancel(sup::dto::uint64 id)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    // Remember the id so a late re-published request for it is ignored.
    RememberFinishedId(id);
    // Drop the request if it is still queued (not yet being prompted).
    for (auto iter = m_queue.begin(); iter != m_queue.end();)
    {
      iter = (iter->id == id) ? m_queue.erase(iter) : std::next(iter);
    }
    // If it is the prompt in progress, interrupt it and suppress its reply.
    if (m_active_id == id)
    {
      m_active_cancelled = true;
      m_job_info_io.Interrupt(id);
    }
  }
  m_cv.notify_all();
}

void WSClientInputDelegator::Enqueue(RequestInfo info)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    if (m_halt)
    {
      return;
    }
    // Ignore re-published requests: an id that is in flight or recently finished is dropped.
    if (IsKnownId(info.id))
    {
      return;
    }
    m_queue.push_back(std::move(info));
  }
  m_cv.notify_all();
}

bool WSClientInputDelegator::IsKnownId(sup::dto::uint64 id) const
{
  if (id == m_active_id)
  {
    return true;
  }
  if (std::any_of(m_queue.begin(), m_queue.end(),
                  [id](const RequestInfo& info) { return info.id == id; }))
  {
    return true;
  }
  return std::find(m_recent_finished.begin(), m_recent_finished.end(), id)
         != m_recent_finished.end();
}

void WSClientInputDelegator::RememberFinishedId(sup::dto::uint64 id)
{
  if (std::find(m_recent_finished.begin(), m_recent_finished.end(), id)
      != m_recent_finished.end())
  {
    return;
  }
  m_recent_finished.push_back(id);
  if (m_recent_finished.size() > kRecentFinishedCapacity)
  {
    m_recent_finished.pop_front();
  }
}

void WSClientInputDelegator::Run()
{
  std::unique_lock<std::mutex> lk{m_mtx};
  while (true)
  {
    m_cv.wait(lk, [this]() { return m_halt || !m_queue.empty(); });
    if (m_halt)
    {
      break;
    }
    auto info = std::move(m_queue.front());
    m_queue.pop_front();
    m_active_id = info.id;
    m_active_cancelled = false;
    lk.unlock();

    // Prompt the local user interface. This blocks until the user responds or the prompt is
    // interrupted (either by a server-side cancellation or on destruction).
    std::string reply;
    if (info.kind == RequestKind::kValue)
    {
      sup::dto::AnyValue value = info.value;
      auto ok = m_job_info_io.GetUserValue(info.id, value, info.description);
      reply = CreateGetUserValueReplyMessage(info.id, ok, value);
    }
    else
    {
      auto choice = m_job_info_io.GetUserChoice(info.id, info.options, info.metadata);
      reply = CreateGetUserChoiceReplyMessage(info.id, choice);
    }

    lk.lock();
    const bool cancelled = m_active_cancelled;
    RememberFinishedId(info.id);
    m_active_id = 0;
    m_active_cancelled = false;
    lk.unlock();

    // A cancelled request was already finished on the server side, so no reply is sent.
    if (!cancelled && info.send)
    {
      info.send(reply);
    }

    lk.lock();
  }
}

}  // namespace oac_tree_server

}  // namespace sup
