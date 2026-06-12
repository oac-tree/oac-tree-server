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

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>

namespace sup
{
namespace oac_tree_server
{

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;

/**
 * @brief WSJobManager is an IJobManager that uses a websocket connection.
 */
class WSJobManager : public IJobManager
{
public:
  WSJobManager(const std::string& host_name, unsigned short port);
  virtual ~WSJobManager();

  std::string GetServerPrefix() const override;
  sup::dto::uint32 GetNumberOfJobs() const override;

  sup::oac_tree::JobInfo GetJobInfo(sup::dto::uint32 job_idx) const override;

  void EditBreakpoint(sup::dto::uint32 job_idx, sup::dto::uint32 instr_idx,
                      bool breakpoint_active) override;

  void SendJobCommand(sup::dto::uint32 job_idx, sup::oac_tree::JobCommand command) override;

  bool WaitForConnection(std::chrono::milliseconds timeout);

private:
  std::string m_server_prefix;
  WsClient m_client;
  std::shared_ptr<WsClient::Connection> m_connection;
  mutable std::mutex m_connection_mutex; // To protect access to m_connection
  std::condition_variable m_connection_cv; // To signal changes to m_connection
  std::future<void> m_client_thread;
  mutable MessageReplyHandler m_reply_handler;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_JOB_MANAGER_H_
