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

#include "ws_client_context.h"

#include <simple-web/asio_compatibility.hpp>

namespace sup
{
namespace oac_tree_server
{

WSClientContext::WSClientContext()
  : m_io_context{std::make_shared<asio::io_context>()}
  , m_thread{}
{
  // The io_context is run by a SINGLE thread. Downstream components rely on this: per-connection
  // asio handlers (on_open/on_message/on_close/on_error) are therefore dispatched serially and
  // never overlap, so message-handling collaborators such as WSClientJob's SnapshotMerger are not
  // synchronized. Running the io_context on multiple threads would break that guarantee (handlers
  // for a connection could run concurrently) and require adding synchronization (e.g. per-
  // connection strands) in those downstream components.
  m_thread = std::thread([io_context = m_io_context]() {
    // The work guard prevents run() from returning while there are temporarily no async operations
    // pending (e.g. before the first connection is established). It is released when the thread
    // returns, after the io_context has been stopped.
    auto work_guard = SimpleWeb::make_work_guard(*io_context);
    io_context->run();
  });
}

WSClientContext::~WSClientContext()
{
  m_io_context->stop();
  if (m_thread.joinable())
  {
    m_thread.join();
  }
}

const std::shared_ptr<asio::io_context>& WSClientContext::GetIoContext() const
{
  return m_io_context;
}

}  // namespace oac_tree_server

}  // namespace sup
