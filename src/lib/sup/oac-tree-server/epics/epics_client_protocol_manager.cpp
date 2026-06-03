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

#include <sup/oac-tree-server/client_job.h>
#include <sup/oac-tree-server/epics_utils.h>
#include <sup/oac-tree-server/exceptions.h>

namespace sup
{
namespace oac_tree_server
{

EPICSClientProtocolManager::EPICSClientProtocolManager(const std::string& service_name)
  : IClientProtocolManager{}
  , m_service_name(service_name)
  , m_job_manager{utils::CreateEPICSJobManager(service_name)}
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
                                         utils::CreateEPICSIOClient, job_info_io);
  }
  catch(const MessageException& e)
  {
    // ignore
  }
  return result;
}

}  // namespace oac_tree_server

}  // namespace sup
