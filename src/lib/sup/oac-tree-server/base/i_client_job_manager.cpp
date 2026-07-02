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

#include <sup/oac-tree-server/i_client_job_manager.h>

namespace sup
{
namespace oac_tree_server
{

IClientJobManager::~IClientJobManager() = default;

bool IsConnected(IClientJobManager& client_mgr)
{
  return client_mgr.WaitForConnection(std::chrono::milliseconds(0));
}

}  // namespace oac_tree_server

}  // namespace sup
