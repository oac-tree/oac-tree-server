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

#include "anyvalue_utils.h"

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/variable.h>
#include <sup/sequencer/workspace.h>

#include <set>

namespace sup
{
namespace auto_server
{
namespace utils
{
WorkspaceInfo CreateWorkspaceInfo(const sequencer::Workspace& ws)
{
  WorkspaceInfo result;
  auto var_names = ws.VariableNames();
  sup::dto::uint32 idx = 0;
  for (const auto& var_name : var_names)
  {
    result.AddVariableInfo(var_name, CreateVariableInfo(ws.GetVariable(var_name), idx));
    ++idx;
  }
  return result;
}

WorkspaceInfo ToWorkspaceInfo(const sup::dto::AnyValue& ws_info_anyvalue)
{
  if (!ValidateWorkspaceInfoAnyValue(ws_info_anyvalue))
  {
    const std::string error = "ToWorkspaceInfo(): wrong format of workspace info AnyValue";
    throw InvalidOperationException(error);
  }
  WorkspaceInfo result;
  auto var_names = ws_info_anyvalue.MemberNames();
  auto n_vars = var_names.size();
  std::set<sup::dto::uint32> used_indices{};
  for (const auto& var_name : var_names)
  {
    const auto& var_av = ws_info_anyvalue[var_name];
    auto idx = var_av[kIndexField].As<sup::dto::uint32>();
    if (idx >= n_vars)
    {
      const std::string error = "ToWorkspaceInfo(): encountered variable index larger or "
                                "equal to number of variables";
      throw InvalidOperationException(error);
    }
    auto var_info = ToVariableInfo(var_av);

    result.AddVariableInfo(var_name, var_info);
    used_indices.insert(idx);
  }
  if (used_indices.size() != n_vars)
  {
    const std::string error = "ToWorkspaceInfo(): duplicate indices found";
    throw InvalidOperationException(error);
  }
  return result;
}

sup::dto::AnyValue ToAnyValue(const WorkspaceInfo& ws_info)
{
  sup::dto::AnyValue result = sup::dto::EmptyStruct();
  const auto& var_infos = ws_info.GetVariableInfos();
  for (const auto& var_info : var_infos)
  {
    auto var_av = ToAnyValue(var_info.second);
    result.AddMember(var_info.first, var_av);
  }
  return result;
}

VariableInfo CreateVariableInfo(const sequencer::Variable* var, sup::dto::uint32 index)
{
  if (var == nullptr)
  {
    const std::string error = "CreateVariableInfo(): called with a nullptr";
    throw InvalidOperationException(error);
  }
  auto var_type = var->GetType();
  std::vector<StringAttribute> attributes = var->GetStringAttributes();
  return VariableInfo{ var_type, index, attributes };
}

VariableInfo ToVariableInfo(const sup::dto::AnyValue& var_info_anyvalue)
{
  if (!ValidateVariableInfoAnyValue(var_info_anyvalue))
  {
    const std::string error = "ToVariableInfo(): wrong format of variable info AnyValue";
    throw InvalidOperationException(error);
  }
  auto var_type = var_info_anyvalue[kVariableInfoTypeField].As<std::string>();
  auto var_idx = var_info_anyvalue[kIndexField].As<sup::dto::uint32>();
  auto& attr_av = var_info_anyvalue[kAttributesField];
  std::vector<StringAttribute> attributes;
  for (const auto& attr_name : attr_av.MemberNames())
  {
    attributes.emplace_back(attr_name, attr_av[attr_name].As<std::string>());
  }
  return VariableInfo{ var_type, var_idx, attributes };
}

sup::dto::AnyValue ToAnyValue(const VariableInfo& var_info)
{
  auto result = kVariableInfoAnyValue;
  result[kVariableInfoTypeField] = var_info.GetType();
  result[kIndexField] = var_info.GetIndex();
  for (const auto& attr : var_info.GetAttributes())
  {
    result[kAttributesField].AddMember(attr.first, attr.second);
  }
  return result;
}

bool ValidateWorkspaceInfoAnyValue(const sup::dto::AnyValue& ws_info)
{
  if (!sup::dto::IsStructValue(ws_info))
  {
    return false;
  }
  auto mem_names = ws_info.MemberNames();
  for (const auto& mem_name : mem_names)
  {
    if (!ValidateVariableInfoAnyValue(ws_info[mem_name]))
    {
      return false;
    }
  }
  return true;
}

bool ValidateVariableInfoAnyValue(const sup::dto::AnyValue& var_info)
{
  if (!ValidateMemberType(var_info, kVariableInfoTypeField, sup::dto::StringType))
  {
    return false;
  }
  if (!ValidateMemberType(var_info, kIndexField, sup::dto::UnsignedInteger32Type))
  {
    return false;
  }
  if (!var_info.HasField(kAttributesField) || !sup::dto::IsStructValue(var_info[kAttributesField]))
  {
    return false;
  }
  const auto& var_attrs = var_info[kAttributesField];
  auto mem_names = var_attrs.MemberNames();
  for (const auto& mem_name : mem_names)
  {
    if (var_attrs[mem_name].GetType() != sup::dto::StringType)
    {
      return false;
    }
  }
  return true;
}

sup::dto::AnyValue BuildWorkspaceInfoAnyValue(const sequencer::Workspace& ws)
{
  sup::dto::AnyValue result = sup::dto::EmptyStruct();
  auto var_names = ws.VariableNames();
  sup::dto::uint32 idx = 0;
  for (const auto& var_name : var_names)
  {
    auto var_av = BuildVariableInfoAnyValue(ws.GetVariable(var_name), idx);
    result.AddMember(var_name, var_av);
    ++idx;
  }
  return result;
}

sup::dto::AnyValue BuildVariableInfoAnyValue(const sequencer::Variable* var, sup::dto::uint32 index)
{
  if (var == nullptr)
  {
    const std::string error = "BuildVariableInfoAnyValue(): called with a nullptr";
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

std::vector<std::string> BuildVariableNameMap(const sup::dto::AnyValue& workspace_info)
{
  if (!ValidateWorkspaceInfoAnyValue(workspace_info))
  {
    const std::string error = "BuildVariableNameMap(): wrong format of workspace info AnyValue";
    throw InvalidOperationException(error);
  }
  auto var_names = workspace_info.MemberNames();
  std::vector<std::string> result(var_names.size());
  std::set<sup::dto::uint32> used_indices{};
  for (const auto& var_name : var_names)
  {
    auto idx = workspace_info[var_name][kIndexField].As<sup::dto::uint32>();
    if (idx >= result.size())
    {
      const std::string error = "BuildVariableNameMap(): encountered variable index larger or "
                                "equal to number of variables";
      throw InvalidOperationException(error);
    }
    result[idx] = var_name;
    used_indices.insert(idx);
  }
  if (used_indices.size() != result.size())
  {
    const std::string error = "BuildVariableNameMap(): duplicate indices found";
    throw InvalidOperationException(error);
  }
  return result;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
