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

#ifndef SUP_AUTO_SERVER_VARIABLE_UTILS_H_
#define SUP_AUTO_SERVER_VARIABLE_UTILS_H_

#include <sup/auto-server/variable_info.h>
#include <sup/auto-server/workspace_info.h>

#include <sup/dto/anyvalue.h>

namespace sup
{
namespace sequencer
{
class Workspace;
class Variable;
}  // namespace sequencer

namespace auto_server
{
namespace utils
{

/**
 * @brief Create a representation of all variables in a workspace, providing their names, types and
 * attributes.
 *
 * @param ws Workspace to use.
 * @return WorkspaceInfo object representing all variables in the given workspace.
 */
WorkspaceInfo CreateWorkspaceInfo(const sequencer::Workspace& ws);

/**
 * @brief Convert the given AnyValue to a list of VariableInfo objects that represent a workspace.
 *
 * @param ws_info_anyvalue AnyValue representation of all workspace variables.
 * @return WorkspaceInfo object.
 * @throw InvalidOperationException when the provided anyvalue has the wrong format.
 */
WorkspaceInfo ToWorkspaceInfo(const sup::dto::AnyValue& ws_info_anyvalue);

/**
 * @brief Convert the given WorkspaceInfo object to an AnyValue.
 *
 * @param ws_info WorkspaceInfo object.
 * @return AnyValue representation of the WorkspaceInfo object.
 */
sup::dto::AnyValue ToAnyValue(const WorkspaceInfo& ws_info);

/**
 * @brief Create a VariableInfo representation of a variable, providing its type,
 * attributes and the variable index used for publishing its status.
 *
 * @param var Variable to represent.
 * @param index Index to put inside the object (referring to the served variable AnyValue status).
 * @return VariableInfo representation.
 * @throw InvalidOperationException when a nullptr is passed.
 */
VariableInfo CreateVariableInfo(const sequencer::Variable* var, sup::dto::uint32 index);

/**
 * @brief Convert the given AnyValue to a VariableInfo object.
 *
 * @param var_info_anyvalue AnyValue representation of a variable.
 * @return VariableInfo object.
 * @throw InvalidOperationException when the provided anyvalue has the wrong format.
 */
VariableInfo ToVariableInfo(const sup::dto::AnyValue& var_info_anyvalue);

/**
 * @brief Convert the given VariableInfo object to an AnyValue.
 *
 * @param var_info VariableInfo object.
 * @return AnyValue representation of the VariableInfo object.
 */
sup::dto::AnyValue ToAnyValue(const VariableInfo& var_info);

/**
 * @brief Validate if the given AnyValue has the right format to be parsed as a WorkspaceInfo
 * object.
 *
 * @details This function only checks that the AnyValue is a structure and each member can be
 * parsed as a VariableInfo object.
 *
 * @param ws_info AnyValue to validate.
 * @return true when the provided AnyValue can be correctly parsed to a WorkspaceInfo object.
 */
bool ValidateWorkspaceInfoAnyValue(const sup::dto::AnyValue& ws_info);

/**
 * @brief Validate if the given AnyValue has the right format to be parsed as a VariableInfo
 * object.
 *
 * @details This function only checks that the AnyValue is a structure and contains the mandatory
 * member fields with the correct type. Any extra fields in the structure are ignored.
 *
 * @param ws_info AnyValue to validate.
 * @return true when the provided AnyValue can be correctly parsed to a VariableInfo object.
 */
bool ValidateVariableInfoAnyValue(const sup::dto::AnyValue& var_info);


// TODO: Remove the following functions

/**
 * @brief Build an AnyValue representation of all variables in the given workspace.
 *
 * @param ws Workspace to represent.
 * @return AnyValue representation.
 */
sup::dto::AnyValue BuildWorkspaceInfoAnyValue(const sequencer::Workspace& ws);

/**
 * @brief Build an AnyValue representation of a variable, providing its type,
 * attributes and the variable index used for publishing its status.
 *
 * @param var Variable to represent.
 * @param index Index to put inside the node (referring to the served variable AnyValue status).
 * @return AnyValue representation.
 * @throw InvalidOperationException when a nullptr is passed.
 */
sup::dto::AnyValue BuildVariableInfoAnyValue(const sequencer::Variable* var, sup::dto::uint32 index);

/**
 * @brief Build an list of variable names from the workspace AnyValue representation, ordered by
 * index. This allows O(1) lookup. It is assumed that all indices are unique and span exactly the
 * range from zero to (number_of_variables -1).
 *
 * @param workspace_info AnyValue representation of all variables in a workspace.
 * @return List of variable names, ordered by index.
 * @throw InvalidOperationException when the assumptions on the input are violated.
 */
std::vector<std::string> BuildVariableNameMap(const sup::dto::AnyValue& workspace_info);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_VARIABLE_UTILS_H_
