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

#include <sup/auto-server/anyvalue_server_interface.h>
#include <sup/auto-server/exceptions.h>

#include <algorithm>

namespace sup
{
namespace auto_server
{

AnyValueServerInterface::~AnyValueServerInterface() = default;

std::set<std::string> GetNames(const AnyValueServerInterface::NameAnyValueSet& name_value_set)
{
  std::set<std::string> result;
  auto func = [](const AnyValueServerInterface::NameAnyValuePair& name_value_pair) {
    return name_value_pair.first;
  };
  std::transform(name_value_set.begin(), name_value_set.end(),
                 std::inserter(result, result.end()), func);
  return result;
}

}  // namespace auto_server

}  // namespace sup
