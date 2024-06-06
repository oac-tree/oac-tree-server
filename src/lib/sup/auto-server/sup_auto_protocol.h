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

// Instruction pv identifier
const std::string kInstructionId = "INSTR-";
// Instruction fields:
const std::string kInstructionType = "sup::instructionType/v1.0";
const std::string kExecStatusField = "exec_status";
const std::string kBreakpointField = "breakpoint";

// Basic instruction AnyValue
extern const sup::dto::AnyValue kInstructionAnyValue;

// Instruction info node fields
const std::string kInstructionInfoNodeType = "sup::instructionNodeInfoType/v1.0";
const std::string kInstructionInfoNodeTypeField = "type";
const std::string kIndexField = "index";
const std::string kAttributesField = "attributes";
const std::string kChildInstructionsField = "children";
const std::string kChildMemberFieldPrefix = "child_";

// Basic instruction info node AnyValue
extern const sup::dto::AnyValue kInstructionInfoNodeAnyValue;

// Variable pv identifier
const std::string kVariableId = "VAR-";
// Variable fields:
const std::string kVariableType = "sup::variableType/v1.0";
const std::string kVariableNameField = "var_name";
const std::string kVariableValueField = "var_value";
const std::string kVariableConnectedField = "var_connected";

// Basic Variable AnyValue
extern const sup::dto::AnyValue kVariableAnyValue;

// Variable info node fields
const std::string kVariableInfoType = "sup::variableInfoType/v1.0";
const std::string kVariableInfoTypeField = "type";

// Basic variable info node AnyValue
extern const sup::dto::AnyValue kVariableInfoAnyValue;

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
const std::string kEditBreakpointCommandFunctionName = "EditBreakpoint";
const std::string kSendJobCommandFunctionName = "SendJobCommand";

// Field names used for the supported functions of automation servers:
const std::string kServerPrefixFieldName = "server_prefix";
const std::string kNumberOfJobsFieldName = "number_of_jobs";
const std::string kJobIndexFieldName = "job_index";
const std::string kJobInfoFieldName = "job_info";
const std::string kInstructionIndexFieldName = "instruction_index";
const std::string kBreakpointActiveFieldName = "breakpoint_active";
const std::string kJobCommandFieldName = "command";

// Field names used for the JobInfo AnyValue:
const std::string kJobPrefixFieldName = "job_prefix";
const std::string kFullNameFieldName = "full_name";
const std::string kNumberOfVarsFieldName = "n_vars";
const std::string kNumberOfInstrFieldName = "n_instr";
const std::string kVariableInfoFieldName = "var_info";
const std::string kInstructionTreeInfoFieldName = "instr_tree_info";

/**
 * @brief Create a PV channel name for the job state from a given prefix.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @return PV channel name for the job state.
 */
std::string GetJobStatePVName(const std::string& prefix);

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
