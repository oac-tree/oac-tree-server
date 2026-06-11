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

#include "ws_message.h"
#include "ws_message_handler.h"

#include <sup/oac-tree-server/exceptions.h>

namespace
{
// TODO: is there a better way than this hardcoded timeout?
constexpr sup::dto::int64 kReplyTimeoutMs = 5000u;
}

namespace sup
{
namespace oac_tree_server
{

WSJobManager::WSJobManager(const std::string& host_name, unsigned short port)
  : m_server_prefix{CreateServerPrefix(host_name, port)}
  , m_client{CreateInfoEndpoint(m_server_prefix)}
  , m_connection{nullptr}
  , m_connection_mutex{}
  , m_client_thread{}
  , m_reply_handler{}
{
  m_client.on_open = [this](std::shared_ptr<WsClient::Connection> connection) {
    std::lock_guard<std::mutex> lk{m_connection_mutex};
    m_connection = connection;
  };
  m_client.on_close = [this](std::shared_ptr<WsClient::Connection> connection,
                             int status, const std::string& reason) {
    (void)connection;
    (void)status;
    (void)reason;
    std::lock_guard<std::mutex> lk{m_connection_mutex};
    m_connection = nullptr;
  };
  m_client.on_message = [this](std::shared_ptr<WsClient::Connection> connection,
                               std::shared_ptr<WsClient::InMessage> in_message) {
    (void)connection;
    // TODO: don't decode the message here, but post it to the reply handler where it will sit
    // in a queue until someone waits for it.
    HandleJobManagerReply(in_message->string(), m_reply_handler);
  };
  m_client.on_error = [](std::shared_ptr<WsClient::Connection> connection,
                         const SimpleWeb::error_code &ec) {
    // TODO: handle error appropriately (e.g., log it, attempt reconnection, etc.)
    (void)ec;
    connection->close();
  };
  // TODO: use a common io_context for the whole WSClientJobManager instead of starting new threads
  // for each WS client.
  m_client_thread = std::async(std::launch::async, [this]() {
    m_client.start();
  });
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
  {
    std::lock_guard<std::mutex> lk{m_connection_mutex};
    if (m_connection)
    {
      m_connection->send(message);
    }
    else
    {
      throw NetworkException("No connection to job manager");
    }
  }
  auto response = m_reply_handler.WaitForReply(id_guard.GetId(),
                                               std::chrono::milliseconds(kReplyTimeoutMs));
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
  {
    std::lock_guard<std::mutex> lk{m_connection_mutex};
    if (m_connection)
    {
      m_connection->send(message);
    }
    else
    {
      throw NetworkException("No connection to job manager");
    }
  }
  auto response = m_reply_handler.WaitForReply(id_guard.GetId(),
                                               std::chrono::milliseconds(kReplyTimeoutMs));
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
  std::lock_guard<std::mutex> lk{m_connection_mutex};
  if (m_connection)
  {
    m_connection->send(message);
  }
}

void WSJobManager::SendJobCommand(sup::dto::uint32 job_idx, sup::oac_tree::JobCommand command)
{
  auto message = CreateSendJobCommandRequestMessage(job_idx, command);
  std::lock_guard<std::mutex> lk{m_connection_mutex};
  if (m_connection)
  {
    m_connection->send(message);
  }
}

}  // namespace oac_tree_server

}  // namespace sup
