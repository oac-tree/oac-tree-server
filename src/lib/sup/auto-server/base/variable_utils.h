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

/**
 * @brief Convert the given AnyValue to a VariableInfo object.
 *
 * @param var_info_anyvalue AnyValue representation of a variable.
 * @return VariableInfo object.
 * @throw InvalidOperationException when the provided anyvalue has the wrong format.
 */
VariableInfo ToVariableInfo(const sup::dto::AnyValue& var_info_anyvalue);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_VARIABLE_UTILS_H_
