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

#include <filesystem>

const std::string kProcedureExtension = ".xml";

namespace
{
bool EndsWith(const std::string& str, const std::string& end)
{
  if (str.size() >= end.size())
  {
    return (str.compare(str.size() - end.size(), end.size(), end) == 0);
  }
  return false;
}
}

namespace sup
{
namespace auto_server
{
namespace utils
{

std::vector<std::string> GetProcedureFilenames(sup::cli::CommandLineParser& parser)
{
  std::vector<std::string> result{};
  if (parser.IsSet("--dir")) {
    auto dir = parser.GetValue<std::string>("--dir");
    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
      if (entry.is_regular_file() && EndsWith(entry.path().string(), kProcedureExtension))
      {
        result.push_back(entry.path());
      }
    }
  }
  auto positional_args = parser.GetPositionalValues();
  result.insert(result.end(), positional_args.begin(), positional_args.end());
  return result;
}
ProcedureList GetProcedureList(sup::cli::CommandLineParser& parser)
{
  ProcedureList result;
  auto filenames = GetProcedureFilenames(parser);
  for (const auto& filename : filenames)
  {
    result.emplace_back(sup::sequencer::ParseProcedureFile(filename));
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
