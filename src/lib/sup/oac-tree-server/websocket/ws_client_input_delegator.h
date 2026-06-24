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

#ifndef SUP_OAC_TREE_SERVER_WS_CLIENT_INPUT_DELEGATOR_H_
#define SUP_OAC_TREE_SERVER_WS_CLIENT_INPUT_DELEGATOR_H_

#include <sup/oac-tree/i_job_info_io.h>

#include <sup/dto/anyvalue.h>

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief Handles incoming user input requests on a websocket client by prompting the local
 * IJobInfoIO and sending the reply back to the server.
 *
 * @details The prompt (IJobInfoIO::GetUserValue/GetUserChoice) is a blocking operation, so it is run
 * on a dedicated worker thread to keep the shared client io_service thread free to receive further
 * messages (including the resends and the cancellation). Because a request is broadcast and
 * periodically re-published, requests are de-duplicated by their id, so a given request prompts the
 * user only once. A cancellation interrupts the matching prompt (via IJobInfoIO::Interrupt) and
 * suppresses its reply.
 */
class WSClientInputDelegator
{
public:
  using SendFunction = std::function<void(const std::string&)>;

  explicit WSClientInputDelegator(sup::oac_tree::IJobInfoIO& job_info_io);
  ~WSClientInputDelegator();

  // No copy or move
  WSClientInputDelegator(const WSClientInputDelegator& other) = delete;
  WSClientInputDelegator(WSClientInputDelegator&& other) = delete;
  WSClientInputDelegator& operator=(const WSClientInputDelegator& other) = delete;
  WSClientInputDelegator& operator=(WSClientInputDelegator&& other) = delete;

  void QueueUserValueRequest(sup::dto::uint64 id, const sup::dto::AnyValue& value,
                             const std::string& description, SendFunction send);
  void QueueUserChoiceRequest(sup::dto::uint64 id, const std::vector<std::string>& options,
                              const sup::dto::AnyValue& metadata, SendFunction send);
  void Cancel(sup::dto::uint64 id);

private:
  enum class RequestKind
  {
    kValue,
    kChoice
  };
  struct RequestInfo
  {
    sup::dto::uint64 id;
    RequestKind kind;
    sup::dto::AnyValue value;
    std::string description;
    std::vector<std::string> options;
    sup::dto::AnyValue metadata;
    SendFunction send;
  };

  void Enqueue(RequestInfo info);
  void Run();
  // Whether a request with this id is currently in flight (queued or being prompted) or was
  // recently finished. Must be called while holding m_mtx.
  bool IsKnownId(sup::dto::uint64 id) const;
  // Record an id as recently finished, keeping only a bounded history. Must hold m_mtx.
  void RememberFinishedId(sup::dto::uint64 id);

  sup::oac_tree::IJobInfoIO& m_job_info_io;
  std::mutex m_mtx;
  std::condition_variable m_cv;
  std::deque<RequestInfo> m_queue;
  // Bounded history of recently finished/cancelled ids, to drop re-published requests that arrive
  // just after a request finished. Only one request is outstanding at a time, so this stays small.
  std::deque<sup::dto::uint64> m_recent_finished;
  sup::dto::uint64 m_active_id;
  bool m_active_cancelled;
  bool m_halt;
  std::thread m_worker;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_CLIENT_INPUT_DELEGATOR_H_
