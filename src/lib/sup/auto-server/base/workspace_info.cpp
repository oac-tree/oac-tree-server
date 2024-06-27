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

#include <sup/auto-server/workspace_info.h>

#include <sup/auto-server/exceptions.h>

#include <algorithm>

namespace sup
{
namespace auto_server
{

WorkspaceInfo::WorkspaceInfo()
  : m_vars{}
{}

WorkspaceInfo::~WorkspaceInfo() = default;

void WorkspaceInfo::AddVariableInfo(const std::string& var_name, const VariableInfo& var_info)
{
  if (!ValidateNewVariableInfo(var_name, var_info))
  {
    const std::string error = "WorkspaceInfo::AddVariableInfo(): trying to add VariableInfo with "
                              "duplicate name or inconsistent index";
    throw InvalidOperationException(error);
  }
  m_vars.emplace_back(var_name, var_info);
}

std::size_t WorkspaceInfo::GetNumberOfVariables() const
{
  return m_vars.size();
}

const std::vector<std::pair<std::string, VariableInfo>>& WorkspaceInfo::GetVariableInfos() const
{
  return m_vars;
}

bool WorkspaceInfo::ValidateNewVariableInfo(const std::string& var_name,
                                            const VariableInfo& var_info) const
{
  auto pred = [var_name](const std::pair<std::string, VariableInfo>& var) {
    return var.first == var_name;
  };
  auto iter = std::find_if(m_vars.begin(), m_vars.end(), pred);
  if (iter != m_vars.end())
  {
    return false;
  }
  return var_info.GetIndex() == m_vars.size();
}

bool operator==(const WorkspaceInfo& left, const WorkspaceInfo& right)
{
  return left.GetVariableInfos() == right.GetVariableInfos();
}

bool operator!=(const WorkspaceInfo& left, const WorkspaceInfo& right)
{
  return !(left == right);
}

}  // namespace auto_server

}  // namespace sup
