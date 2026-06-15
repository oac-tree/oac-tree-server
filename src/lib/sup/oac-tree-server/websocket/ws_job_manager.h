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

#ifndef SUP_OAC_TREE_SERVER_WS_JOB_MANAGER_H_
#define SUP_OAC_TREE_SERVER_WS_JOB_MANAGER_H_

#include <sup/oac-tree-server/base/message_reply_handler.h>
#include <sup/oac-tree-server/i_job_manager.h>

#include <simple-web/client_ws.hpp>

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>

namespace sup
{
namespace oac_tree_server
{

class WSClientContext;

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;

/**
 * @brief WSJobManager is an IJobManager that uses a websocket connection.
 */
class WSJobManager : public IJobManager
{
public:
  /**
   * @brief Construct a WSJobManager with a configurable timeout for waiting on server replies.
   * @param host_name Host name of the server.
   * @param port Port of the server.
   * @param reply_timeout Maximum duration to wait for a reply to a request before failing.
   */
  WSJobManager(const std::string& host_name, unsigned short port,
               std::chrono::milliseconds reply_timeout);

  virtual ~WSJobManager();

  std::string GetServerPrefix() const override;
  sup::dto::uint32 GetNumberOfJobs() const override;

  sup::oac_tree::JobInfo GetJobInfo(sup::dto::uint32 job_idx) const override;

  void EditBreakpoint(sup::dto::uint32 job_idx, sup::dto::uint32 instr_idx,
                      bool breakpoint_active) override;

  void SendJobCommand(sup::dto::uint32 job_idx, sup::oac_tree::JobCommand command) override;

  bool WaitForConnection(std::chrono::milliseconds timeout);

  /**
   * @brief Returns the shared client context (io_context and its worker thread) used by this job
   * manager, so that client jobs created against the same server can share it.
   */
  std::shared_ptr<WSClientContext> GetClientContext() const;

private:
  /**
   * @brief Return the current connection (or nullptr) under the connection mutex, so callers can
   * send without holding the mutex across the (non-blocking) send call.
   */
  std::shared_ptr<WsClient::Connection> GetConnection() const;

  std::string m_server_prefix;
  std::shared_ptr<WSClientContext> m_context;
  WsClient m_client;
  std::shared_ptr<WsClient::Connection> m_connection;
  mutable std::mutex m_connection_mutex; // To protect access to m_connection
  std::condition_variable m_connection_cv; // To signal changes to m_connection
  mutable MessageReplyHandler m_reply_handler;
  std::chrono::milliseconds m_reply_timeout;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_JOB_MANAGER_H_
