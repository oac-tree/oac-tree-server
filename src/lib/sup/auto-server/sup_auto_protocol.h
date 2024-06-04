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
#include <sup/sequencer/job_states.h>

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
const std::string kInstructionId = "INSTR-";
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
const std::string kVariableNameField = "var_name";
const std::string kVariableValueField = "var_value";
const std::string kVariableConnectedField = "var_connected";

// Basic Variable AnyValue
extern const sup::dto::AnyValue kVariableAnyValue;

// Job state postfix:
const std::string kJobStateId = "STATE";
// Job state type name and fields:
const std::string kJobStateType = "sup::jobState/v1.0";
const std::string kJobStateField = "state";

// Basic job state AnyValue
extern const sup::dto::AnyValue kJobStateAnyValue;

// Automation servers will report the following type and version:
const std::string kAutomationServerProtocolServerType = "SUP::AutomationServerProtocol";
const std::string kAutomationServerProtocolServerVersion = "1.0";

// Supported function names for automation servers:
const std::string kGetServerPrefixFunctionName = "GetServerPrefix";
const std::string kGetNumberOfJobsFunctionName = "GetNumberOfJobs";
const std::string kGetJobInfoFunctionName = "GetJobInfo";
const std::string kSendJobCommandFunctionName = "SendJobCommand";

// Field names used for the supported functions of automation servers:
const std::string kServerPrefixFieldName = "server_prefix";
const std::string kNumberOfJobsFieldName = "number_of_jobs";
const std::string kJobIndexFieldName = "job_index";
const std::string kJobInfoFieldName = "job_info";
const std::string kJobCommandFieldName = "command";

// Field names used for the JobInfo AnyValue:
const std::string kJobPrefixFieldName = "job_prefix";
const std::string kFullNameFieldName = "full_name";
const std::string kNumberOfVarsFieldName = "n_vars";

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
 * @brief Create a PV channel name for an instruction with the given index and prefix.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @param index Index of the instruction inside the procedure.
 * @return PV channel name for the instruction.
 */
std::string GetInstructionPVName(const std::string& prefix, sup::dto::uint32 index);

/**
 * @brief Create a PV channel name for a variable with the given index and prefix.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @param index Index of the variable inside the workspace.
 * @return PV channel name for the variable.
 */
std::string GetVariablePVName(const std::string& prefix, sup::dto::uint32 index);

/**
 * @brief Create an AnyValue representing the given job state.
 *
 * @param state Job state to encode in AnyValue.
 * @return AnyValue representing the given job state.
 */
sup::dto::AnyValue GetJobStateValue(sup::sequencer::JobState state);

/**
 * @brief Pack a variable's name, value and connected state into a base64 encoded AnyValue.
 *
 * @param name Variable's name.
 * @param value Variable's value.
 * @param connected Variable's connected status.
 *
 * @return Packed and encoded AnyValue.
 */
sup::dto::AnyValue EncodeVariableInfo(const std::string& name, const dto::AnyValue& value,
                                      bool connected);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SUP_AUTO_PROTOCOL_H_
