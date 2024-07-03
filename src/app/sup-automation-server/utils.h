/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVER
 *
 * Description   : SUP automation server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_AUTO_SERVER_SUP_AUTOMATION_SERVER_UTILS_H_
#define SUP_AUTO_SERVER_SUP_AUTOMATION_SERVER_UTILS_H_

#include <sup/auto-server/i_anyvalue_manager_registry.h>

#include <sup/cli/command_line_parser.h>
#include <sup/sequencer/procedure.h>

#include <memory>

namespace sup
{
namespace auto_server
{
namespace utils
{
using ProcedureList = std::vector<std::unique_ptr<sup::sequencer::Procedure>>;

ProcedureList GetProcedureList(sup::cli::CommandLineParser& parser);

std::unique_ptr<IAnyValueManagerRegistry> CreateAnyValueManagerRegistry(std::size_t n_managers);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SUP_AUTOMATION_SERVER_UTILS_H_
