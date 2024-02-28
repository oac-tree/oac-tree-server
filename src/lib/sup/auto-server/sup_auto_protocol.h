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

#ifndef SUP_AUTO_SERVER_SUP_AUTO_PROTOCOL_H_
#define SUP_AUTO_SERVER_SUP_AUTO_PROTOCOL_H_

#include <sup/dto/anyvalue.h>

#include <string>

namespace sup
{
namespace auto_server
{
// Prefix suggestion: e.g. for CC2D loop:
// "CSW-S2D-AUTO:"
// Prefix for a specific procedure:
// "CSW-S2D-AUTO-STARTUP:

// Instruction tree postfix:
const std::string kInstructionTreeId = "INST";
// Instruction tree node fields:
const std::string kInstructionNodeType = "sup::instructionNode/v1.0";
const std::string kExecStatusField = "exec_status";
const std::string kBreakpointField = "breakpoint";
const std::string kChildrenField = "children";

// Basic instruction node AnyValue
extern const sup::dto::AnyValue kInstructionAnyValue;

// Variable pv identifier
const std::string kVariableId = "VAR-";
// Variable fields:
const std::string kVariableType = "sup::variableType/v1.0";

// Basic Variable AnyValue
extern const sup::dto::AnyValue kVariableAnyValue;

// Job state postfix:
const std::string kJobStateId = "STATE";
// Job state type name and fields:
const std::string kJobStateType = "sup::jobState/v1.0";
const std::string kJobStateField = "state";

// Basic job state AnyValue
extern const sup::dto::AnyValue kJobStateAnyValue;

/**
 * @brief Create a PV channel name for the job state from a given prefix.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @return PV channel name for the job state.
 */
std::string GetJobStatePVName(const std::string& prefix);

/**
 * @brief Create a PV channel name for the instruction tree from a given prefix.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @return PV channel name for the instruction tree.
 */
std::string GetInstructionTreePVName(const std::string& prefix);

/**
 * @brief Create a PV channel name for a variable with the given index and prefix.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @param index Index of the variable inside the workspace.
 * @return PV channel name for the variable.
 */
std::string GetVariablePVName(const std::string& prefix, sup::dto::uint32 index);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SUP_AUTO_PROTOCOL_H_
