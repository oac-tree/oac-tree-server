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

#include "epics_client_protocol_manager.h"

#include "epics_io_client.h"

#include <sup/oac-tree-server/exceptions.h>

#include <sup/oac-tree-server/base/automation_client_stack.h>
#include <sup/oac-tree-server/base/client_job.h>
#include <sup/oac-tree-server/base/oac_tree_protocol.h>

#include <sup/epics/epics_protocol_factory.h>

namespace sup
{
namespace oac_tree_server
{

EPICSClientProtocolManager::EPICSClientProtocolManager(const std::string& service_name)
  : IClientProtocolManager{}
  , m_service_name(service_name)
  , m_job_manager{CreateEPICSJobManager(service_name)}
{}

EPICSClientProtocolManager::~EPICSClientProtocolManager() = default;

IJobManager& EPICSClientProtocolManager::GetJobManager()
{
  return *m_job_manager;
}

std::unique_ptr<sup::oac_tree::IJob> EPICSClientProtocolManager::CreateJob(
  sup::dto::uint32 job_idx, sup::oac_tree::IJobInfoIO &job_info_io)
{
  std::unique_ptr<sup::oac_tree::IJob> result{};
  try
  {
    result = std::make_unique<ClientJob>(*m_job_manager, job_idx,
                                         CreateEPICSIOClient, job_info_io);
  }
  catch(const MessageException& e)
  {
    // ignore
  }
  return result;
}

std::unique_ptr<IJobManager> CreateEPICSJobManager(const std::string& server_name)
{
  sup::protocol::ProtocolRPCClientConfig protocol_config{};
  auto info_config = sup::epics::GetDefaultRPCClientConfig(server_name);
  auto info_protocol = sup::epics::CreateEPICSRPCClientStack(info_config, protocol_config);
  auto control_server_name = GetControlServerName(server_name);
  auto control_config = sup::epics::GetDefaultRPCClientConfig(control_server_name);
  auto control_protocol = sup::epics::CreateEPICSRPCClientStack(control_config, protocol_config);
  auto result = std::make_unique<AutomationClientStack>(std::move(info_protocol),
                                                        std::move(control_protocol));
  return result;
}

std::unique_ptr<IAnyValueIO> CreateEPICSIOClient(IAnyValueManager& av_mgr)
{
  return std::make_unique<EPICSIOClient>(av_mgr);
}

}  // namespace oac_tree_server

}  // namespace sup
