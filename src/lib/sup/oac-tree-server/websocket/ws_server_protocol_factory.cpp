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

#include "ws_server_protocol_factory.h"

#include <sup/oac-tree-server/oac_tree_protocol.h>
#include <sup/oac-tree-server/server_job_info_io.h>

namespace sup
{
namespace oac_tree_server
{

WSServerProtocolFactory::WSServerProtocolFactory()
  : IServerProtocolFactory{}
{}

WSServerProtocolFactory::~WSServerProtocolFactory() = default;

std::unique_ptr<IMainServer> WSServerProtocolFactory::CreateMainServer(
  const std::string& service_name, IJobManager& job_manager)
{
  (void)service_name;
  (void)job_manager;
  return {};
}

std::unique_ptr<sup::oac_tree::IJobInfoIO> WSServerProtocolFactory::CreateJobInfoIO(
  const std::string& server_prefix, sup::dto::uint32 job_idx, sup::dto::uint32 n_vars)
{
  (void)server_prefix;
  (void)job_idx;
  (void)n_vars;
  return {};
}

}  // namespace oac_tree_server

}  // namespace sup
