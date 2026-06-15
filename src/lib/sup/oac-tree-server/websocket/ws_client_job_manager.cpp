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

#include "ws_client_job_manager.h"

#include "ws_client_job.h"
#include "ws_job_manager.h"

#include <sup/oac-tree-server/exceptions.h>

namespace sup
{
namespace oac_tree_server
{

WSClientJobManager::WSClientJobManager(const std::string& host_name, unsigned short port)
  : IClientJobManager{}
  , m_host_name(host_name)
  , m_port(port)
  , m_job_manager{CreateWSJobManager(host_name, port)}
{}

WSClientJobManager::~WSClientJobManager() = default;

std::string WSClientJobManager::GetServerPrefix() const
{
  return m_job_manager->GetServerPrefix();
}

sup::dto::uint32 WSClientJobManager::GetNumberOfJobs() const
{
  return m_job_manager->GetNumberOfJobs();
}

sup::oac_tree::JobInfo WSClientJobManager::GetJobInfo(sup::dto::uint32 job_idx) const
{
  return m_job_manager->GetJobInfo(job_idx);
}

void WSClientJobManager::EditBreakpoint(
  sup::dto::uint32 job_idx, sup::dto::uint32 instr_idx, bool breakpoint_active)
{
  m_job_manager->EditBreakpoint(job_idx, instr_idx, breakpoint_active);
}

void WSClientJobManager::SendJobCommand(sup::dto::uint32 job_idx,
                                                sup::oac_tree::JobCommand command)
{
  m_job_manager->SendJobCommand(job_idx, command);
}

std::unique_ptr<IClientJob> WSClientJobManager::CreateJob(
  sup::dto::uint32 job_idx, sup::oac_tree::IJobInfoIO &job_info_io)
{
  return std::make_unique<WSClientJob>(*m_job_manager, job_idx, job_info_io,
                                       m_job_manager->GetClientContext());
}

bool WSClientJobManager::WaitForConnection(std::chrono::milliseconds timeout)
{
  return m_job_manager->WaitForConnection(timeout);
}

std::unique_ptr<WSJobManager> CreateWSJobManager(const std::string& host_name, unsigned short port)
{
  return std::make_unique<WSJobManager>(host_name, port);
}

}  // namespace oac_tree_server

}  // namespace sup
