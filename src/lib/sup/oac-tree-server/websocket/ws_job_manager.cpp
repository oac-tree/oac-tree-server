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

#include <sup/oac-tree-server/exceptions.h>

namespace sup
{
namespace oac_tree_server
{

WSJobManager::WSJobManager(const std::string& host_name, unsigned short port)
  : m_client{"ws://" + host_name + ":" + std::to_string(port) + endpoints::kJobManagerEndpoint}
  , m_connection{nullptr}
  , m_connection_mutex{}
  , m_client_thread{}
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
    (void)in_message;
  };
  m_client.on_error = [](std::shared_ptr<WsClient::Connection> connection,
                         const SimpleWeb::error_code &ec) {
    // TODO: handle error appropriately (e.g., log it, attempt reconnection, etc.)
    (void)ec;
    connection->close();
  };
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
  throw InvalidOperationException("GetServerPrefix is not implemented yet");
}

sup::dto::uint32 WSJobManager::GetNumberOfJobs() const
{
  throw InvalidOperationException("GetNumberOfJobs is not implemented yet");
}

sup::oac_tree::JobInfo WSJobManager::GetJobInfo(sup::dto::uint32 job_idx) const
{
  (void)job_idx;
  throw InvalidOperationException("GetJobInfo is not implemented yet");
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
