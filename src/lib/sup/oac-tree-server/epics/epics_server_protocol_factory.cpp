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

#include "epics_server_protocol_factory.h"
#include "epics_main_server.h"

#include <sup/oac-tree-server/base/oac_tree_protocol.h>
#include <sup/oac-tree-server/base/server_job_info_io.h>

namespace sup
{
namespace oac_tree_server
{

EPICSServerProtocolFactory::EPICSServerProtocolFactory(const std::string& service_name,
                                                       sup::dto::uint32 n_managers)
  : IServerProtocolFactory{}
  , m_service_name(service_name)
  , m_av_mgr_registry(n_managers)
{}

EPICSServerProtocolFactory::~EPICSServerProtocolFactory() = default;

std::string EPICSServerProtocolFactory::GetServerId() const
{
  return m_service_name;
}

std::unique_ptr<IMainServer> EPICSServerProtocolFactory::CreateMainServer(
  IJobManager& job_manager)
{
  return std::make_unique<EPICSMainServer>(m_service_name, job_manager);
}

std::unique_ptr<sup::oac_tree::IJobInfoIO> EPICSServerProtocolFactory::CreateJobInfoIO(
  sup::dto::uint32 job_idx, sup::dto::uint32 n_vars)
{
  const auto job_prefix = CreateJobPrefix(m_service_name, job_idx);
  return std::make_unique<ServerJobInfoIO>(job_prefix, n_vars,
                                           m_av_mgr_registry.GetAnyValueManager(job_idx));
}

}  // namespace oac_tree_server

}  // namespace sup
