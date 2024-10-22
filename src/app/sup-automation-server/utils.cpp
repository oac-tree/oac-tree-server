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

#include "utils.h"

#include <sup/auto-server/epics/epics_anyvalue_manager_registry.h>

#include <sup/sequencer/sequence_parser.h>

namespace sup
{
namespace auto_server
{
namespace utils
{

ProcedureList GetProcedureList(sup::cli::CommandLineParser& parser)
{
  ProcedureList result;
  auto positional_args = parser.GetPositionalValues();
  for (size_t idx = 0; idx < positional_args.size(); ++idx)
  {
    result.emplace_back(sup::sequencer::ParseProcedureFile(positional_args[idx]));
  }
  return result;
}

std::unique_ptr<IAnyValueManagerRegistry> CreateAnyValueManagerRegistry(std::size_t n_managers)
{
  std::unique_ptr<IAnyValueManagerRegistry> result{new EPICSAnyValueManagerRegistry{n_managers}};
  return result;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
