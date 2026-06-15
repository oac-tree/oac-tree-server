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

#include "ws_job_manager.h"

#include "ws_client_context.h"
#include "ws_message.h"
#include "ws_message_handler.h"

#include <sup/oac-tree-server/exceptions.h>

namespace sup
{
namespace oac_tree_server
{

WSJobManager::WSJobManager(const std::string& host_name, unsigned short port,
                           std::chrono::milliseconds reply_timeout)
  : m_server_prefix{CreateServerPrefix(host_name, port)}
  , m_context{std::make_shared<WSClientContext>()}
  , m_client{CreateInfoEndpoint(m_server_prefix)}
  , m_connection{nullptr}
  , m_connection_mutex{}
  , m_connection_cv{}
  , m_reply_handler{}
  , m_reply_timeout{reply_timeout}
{
  m_client.on_open = [this](std::shared_ptr<WsClient::Connection> connection) {
    {
      std::lock_guard<std::mutex> lk{m_connection_mutex};
      m_connection = connection;
    }
    m_connection_cv.notify_all();
  };
  m_client.on_close = [this](std::shared_ptr<WsClient::Connection> connection,
                             int status, const std::string& reason) {
    (void)connection;
    (void)status;
    (void)reason;
    {
      std::lock_guard<std::mutex> lk{m_connection_mutex};
      m_connection = nullptr;
    }
    m_connection_cv.notify_all();
  };
  m_client.on_message = [this](std::shared_ptr<WsClient::Connection> connection,
                               std::shared_ptr<WsClient::InMessage> in_message) {
    (void)connection;
    // TODO: don't decode the message here, but post it to the reply handler where it will sit
    // in a queue until someone waits for it.
    HandleJobManagerReply(in_message->string(), m_reply_handler);
  };
  m_client.on_error = [this](std::shared_ptr<WsClient::Connection> connection,
                             const SimpleWeb::error_code &ec) {
    // SimpleWeb only reports terminal errors here, and a force close() does not trigger on_close,
    // so reset the connection state to reflect the dropped connection (otherwise it would stay
    // stale after a network error) and close the dead socket.
    (void)ec;
    {
      std::lock_guard<std::mutex> lk{m_connection_mutex};
      m_connection = nullptr;
    }
    m_connection_cv.notify_all();
    connection->close();
  };
  // Share the manager's io_context so all clients are serviced by a single worker thread. With an
  // external io_context, start() only queues the connection and returns immediately instead of
  // running the context itself.
  m_client.io_service = m_context->GetIoContext();
  m_client.start();
}

WSJobManager::~WSJobManager()
{
  m_client.stop();
  // m_client_thread will be joined automatically when it goes out of scope
}

std::string WSJobManager::GetServerPrefix() const
{
  return m_server_prefix;
}

sup::dto::uint32 WSJobManager::GetNumberOfJobs() const
{
  auto id_guard = m_reply_handler.CreateNewMessageId();
  auto message = CreateGetNumberOfJobsRequestMessage(id_guard.GetId());
  auto connection = GetConnection();
  if (!connection)
  {
    throw NetworkException("No connection to job manager");
  }
  connection->send(message);
  auto response = m_reply_handler.WaitForReply(id_guard.GetId(), m_reply_timeout);
  if (!response)
  {
    throw NetworkException("No response from job manager");
  }
  auto number_of_jobs = ParseNumberOfJobsReply(response.value());
  if (!number_of_jobs)
  {
    throw NetworkException("Failed to parse number of jobs from job manager response");
  }
  return number_of_jobs.value();
}

sup::oac_tree::JobInfo WSJobManager::GetJobInfo(sup::dto::uint32 job_idx) const
{
  auto id_guard = m_reply_handler.CreateNewMessageId();
  auto message = CreateGetJobInfoRequestMessage(id_guard.GetId(), job_idx);
  auto connection = GetConnection();
  if (!connection)
  {
    throw NetworkException("No connection to job manager");
  }
  connection->send(message);
  auto response = m_reply_handler.WaitForReply(id_guard.GetId(), m_reply_timeout);
  if (!response)  {
    throw NetworkException("No response from job manager");
  }
  auto job_info = ParseJobInfoReply(response.value());
  if (!job_info)
  {
    throw NetworkException("Failed to parse job info from job manager response");
  }
  return job_info.value();
}

void WSJobManager::EditBreakpoint(sup::dto::uint32 job_idx, sup::dto::uint32 instr_idx,
                                  bool breakpoint_active)
{
  auto message = CreateEditBreakpointRequestMessage(job_idx, instr_idx, breakpoint_active);
  if (auto connection = GetConnection())
  {
    connection->send(message);
  }
}

void WSJobManager::SendJobCommand(sup::dto::uint32 job_idx, sup::oac_tree::JobCommand command)
{
  auto message = CreateSendJobCommandRequestMessage(job_idx, command);
  if (auto connection = GetConnection())
  {
    connection->send(message);
  }
}

bool WSJobManager::WaitForConnection(std::chrono::milliseconds timeout)
{
  std::unique_lock<std::mutex> lk{m_connection_mutex};
  return m_connection_cv.wait_for(lk, timeout, [this] { return m_connection != nullptr; });
}

std::shared_ptr<WSClientContext> WSJobManager::GetClientContext() const
{
  return m_context;
}

std::shared_ptr<WsClient::Connection> WSJobManager::GetConnection() const
{
  std::lock_guard<std::mutex> lk{m_connection_mutex};
  return m_connection;
}

}  // namespace oac_tree_server

}  // namespace sup
