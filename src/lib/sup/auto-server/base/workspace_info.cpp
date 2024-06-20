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
