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
 * Copyright (c) : 2010-2025 ITER Organization,
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

#ifndef SUP_OAC_TREE_SERVER_EPICS_CLIENT_UTILS_H_
#define SUP_OAC_TREE_SERVER_EPICS_CLIENT_UTILS_H_

#include <sup/oac-tree-server/i_anyvalue_io.h>
#include <sup/oac-tree-server/i_anyvalue_manager_registry.h>
#include <sup/oac-tree-server/i_job_manager.h>

#include <memory>
#include <string>

namespace sup
{
namespace oac_tree_server
{
namespace utils
{

std::unique_ptr<IAnyValueIO> CreateEPICSIOClient(IAnyValueManager& av_mgr);

std::unique_ptr<IJobManager> CreateEPICSJobManager(const std::string& server_name);

std::unique_ptr<IAnyValueManagerRegistry> CreateEPICSAnyValueManagerRegistry(
    sup::dto::uint32 n_managers);

}  // namespace utils

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_EPICS_CLIENT_UTILS_H_
