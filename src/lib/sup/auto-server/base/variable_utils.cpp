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

#include "variable_utils.h"

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/variable.h>
#include <sup/sequencer/workspace.h>

namespace sup
{
namespace auto_server
{
namespace utils
{
sup::dto::AnyValue BuildWorkspaceInfo(const sequencer::Workspace& ws)
{
  sup::dto::AnyValue result = sup::dto::EmptyStruct();
  auto var_names = ws.VariableNames();
  sup::dto::uint32 idx = 0;
  for (const auto& var_name : var_names)
  {
    auto var_av = BuildVariableInfo(ws.GetVariable(var_name), idx);
    result.AddMember(var_name, var_av);
    ++idx;
  }
  return result;
}

sup::dto::AnyValue BuildVariableInfo(const sequencer::Variable* var, sup::dto::uint32 index)
{
  if (var == nullptr)
  {
    const std::string error = "BuildVariableInfo(): called with a nullptr";
    throw InvalidOperationException(error);
  }
  auto result = kVariableInfoAnyValue;
  result[kVariableInfoTypeField] = var->GetType();
  result[kIndexField] = index;
  for (const auto& attr : var->GetStringAttributes())
  {
    result[kAttributesField].AddMember(attr.first, attr.second);
  }
  return result;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
