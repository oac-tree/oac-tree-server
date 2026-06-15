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

#ifndef SUP_OAC_TREE_SERVER_WS_CLIENT_CONTEXT_H_
#define SUP_OAC_TREE_SERVER_WS_CLIENT_CONTEXT_H_

#include <memory>
#include <thread>

// Forward declaration of the (standalone) Asio io_context to keep Asio headers out of this header.
// This matters because consumers such as the unit tests include this header but are not compiled
// with ASIO_STANDALONE; pulling in <asio.hpp>/<boost/asio.hpp> here would leak Asio (and its link
// dependencies) into them. The library itself is always built with standalone Asio, where
// SimpleWeb::io_context is an alias for asio::io_context.
namespace asio
{
class io_context;
}

namespace sup
{
namespace oac_tree_server
{
/**
 * @brief Owns a single Asio io_context together with the worker thread that runs it.
 *
 * All WebSocket clients of a WSClientJobManager (the job manager's info client and the per-job
 * clients) share a single WSClientContext, so that the number of threads servicing the clients is
 * fixed at one instead of growing with the number of jobs.
 *
 * The context is held through a std::shared_ptr by every client that uses it. Since a client job
 * can outlive the manager that created it, this guarantees the io_context (and its thread) stays
 * alive until the last client using it is destroyed.
 */
class WSClientContext
{
public:
  WSClientContext();
  ~WSClientContext();

  // No copy or move
  WSClientContext(const WSClientContext&) = delete;
  WSClientContext(WSClientContext&&) = delete;
  WSClientContext& operator=(const WSClientContext&) = delete;
  WSClientContext& operator=(WSClientContext&&) = delete;

  /**
   * @brief Returns the shared io_context to assign to a client's io_service member before calling
   * start().
   */
  const std::shared_ptr<asio::io_context>& GetIoContext() const;

private:
  std::shared_ptr<asio::io_context> m_io_context;
  std::thread m_thread;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_CLIENT_CONTEXT_H_
