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
sup::dto::AnyValue BuildWorkspaceInfo(const sequencer::Workspace& ws);

/**
 * @brief Build an AnyValue representation of a variable, providing its type,
 * attributes and the variable index used for publishing its status.
 *
 * @param var Variable to represent.
 * @param index Index to put inside the node (referring to the served variable AnyValue status).
 * @return AnyValue representation.
 */
sup::dto::AnyValue BuildVariableInfo(const sequencer::Variable* var, sup::dto::uint32 index);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_VARIABLE_UTILS_H_
