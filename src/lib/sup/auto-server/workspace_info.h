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

#ifndef SUP_AUTO_SERVER_WORKSPACE_INFO_H_
#define SUP_AUTO_SERVER_WORKSPACE_INFO_H_

#include <sup/auto-server/variable_info.h>

#include <sup/dto/basic_scalar_types.h>

#include <string>
#include <utility>
#include <vector>

namespace sup
{
namespace auto_server
{
/**
 * @brief The WorkspaceInfo class represents the static information of a workspace, which consists
 * mainly of mapping variable names to VariableInfo objects.
 *
 * @details When objects of this type are created from a real Workspace object, every VariableInfo,
 * representing a single Variable in the Workspace, will receive a unique index from the range
 * [0, N-1], with N the number of Variables. These indices is encoded inside the VariableInfo
 * objects, but are also reflected by their position inside the list that can be retrieved with
 * GetVariableInfos(). This duplication of information allows two-way lookup between VariableInfo
 * objects and their indices.
 */
class WorkspaceInfo
{
public:
  WorkspaceInfo();
  ~WorkspaceInfo();

  void AddVariableInfo(const std::string& var_name, const VariableInfo& var_info);

  std::size_t GetNumberOfVariables() const;

  const std::vector<std::pair<std::string, VariableInfo>>& GetVariableInfos() const;

private:
  std::vector<std::pair<std::string, VariableInfo>> m_vars;
};

/**
 * @brief Compare two WorkspaceInfo objects for equality.
 */
bool operator==(const WorkspaceInfo& left, const WorkspaceInfo& right);

/**
 * @brief Compare two WorkspaceInfo objects for inequality.
 */
bool operator!=(const WorkspaceInfo& left, const WorkspaceInfo& right);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_WORKSPACE_INFO_H_
