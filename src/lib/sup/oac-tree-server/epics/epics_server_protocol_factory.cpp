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

#include <sup/oac-tree-server/oac_tree_protocol.h>
#include <sup/oac-tree-server/server_job_info_io.h>

namespace sup
{
namespace oac_tree_server
{

EPICSServerProtocolFactory::EPICSServerProtocolFactory(sup::dto::uint32 n_managers)
  : IServerProtocolFactory{}
  , m_av_mgr_registry(n_managers)
{}

EPICSServerProtocolFactory::~EPICSServerProtocolFactory() = default;

std::unique_ptr<IMainServer> EPICSServerProtocolFactory::CreateMainServer(
  const std::string& service_name, AutomationServer& automation_server)
{
  return std::make_unique<EPICSMainServer>(service_name, automation_server);
}

std::unique_ptr<sup::oac_tree::IJobInfoIO> EPICSServerProtocolFactory::CreateJobInfoIO(
  const std::string& server_prefix, sup::dto::uint32 job_idx, sup::dto::uint32 n_vars)
{
  const auto job_prefix = CreateJobPrefix(server_prefix, job_idx);
  return std::make_unique<ServerJobInfoIO>(job_prefix, n_vars,
                                           m_av_mgr_registry.GetAnyValueManager(job_idx));
}

}  // namespace oac_tree_server

}  // namespace sup
