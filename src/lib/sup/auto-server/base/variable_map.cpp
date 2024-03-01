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

#include <sup/auto-server/variable_map.h>

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/workspace.h>

namespace sup
{
namespace auto_server
{

VariableMap::VariableMap(const std::string& prefix, const sequencer::Workspace& workspace)
  : m_variable_map{}
{
  InitializeMap(prefix, workspace);
}

VariableMap::~VariableMap() = default;

std::string VariableMap::FindVariableIdentifier(const std::string& var_name) const
{
  auto iter = m_variable_map.find(var_name);
  if (iter == m_variable_map.end())
  {
    std::string message = "VariableMap::FindVariableIdentifier(): unknown variable name [" +
      var_name + "]";
    throw InvalidOperationException(message);
  }
  return iter->second;
}

std::map<std::string, std::string> VariableMap::GetVariableMapping() const
{
  return m_variable_map;
}

void VariableMap::InitializeMap(const std::string& prefix, const sequencer::Workspace& workspace)
{
  auto var_names = workspace.VariableNames();
  for (sup::dto::uint32 idx = 0; idx < var_names.size(); ++idx)
  {
    m_variable_map[var_names[idx]] = GetVariablePVName(prefix, idx);
  }
}

}  // namespace auto_server

}  // namespace sup
