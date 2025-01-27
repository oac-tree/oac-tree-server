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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_OAC_TREE_SERVER_SUP_AUTOMATION_SERVER_UTILS_H_
#define SUP_OAC_TREE_SERVER_SUP_AUTOMATION_SERVER_UTILS_H_

#include <sup/oac-tree-server/i_anyvalue_manager_registry.h>

#include <sup/cli/command_line_parser.h>
#include <sup/oac-tree/procedure.h>

#include <memory>

namespace sup
{
namespace oac_tree_server
{
namespace utils
{
using ProcedureList = std::vector<std::unique_ptr<sup::oac_tree::Procedure>>;

std::vector<std::string> GetProcedureFilenames(sup::cli::CommandLineParser& parser);

ProcedureList GetProcedureList(sup::cli::CommandLineParser& parser);

}  // namespace utils

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_SUP_AUTOMATION_SERVER_UTILS_H_
