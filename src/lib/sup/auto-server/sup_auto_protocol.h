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
 * Copyright (c) : 2010-2025 ITER Organization,
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

#include <sup/protocol/protocol_result.h>

#include <string>

namespace sup
{
namespace auto_server
{

// Prefix suggestion: e.g. for CC2D loop:
// "CSW-S2D-AUTO:"

// Control server identifier
const std::string kControlServerId = "CONTROL:";

// Instruction pv identifier
const std::string kInstructionId = "INSTR-";

// Variable pv identifier
const std::string kVariableId = "VAR-";
const std::string kVariableType = "sup::variableType/v1.0";
// Variable fields:
const std::string kVariableValueField = "var_value";
const std::string kVariableConnectedField = "var_connected";

// Basic Variable AnyValue
extern const sup::dto::AnyValue kVariableAnyValue;

// Basic input request AnyValue
extern const sup::dto::AnyValue kInputRequestAnyValue;

// User input server and request names:
const std::string kInputServerName = "INPUT";
const std::string kInputRequestName = "-REQ";
const std::string kInputRequestType = "sup::inputRequestType/v1.0";
// Input request fields:
const std::string kInputRequestIndexField = "idx";
const std::string kInputRequestTypeField = "request_type";
const std::string kInputRequestMetadataField = "metadata";
const std::string kInputRequestInputTypeField = "input_type";
// Input reply fields:
const std::string kInputReplyIndexField = "idx";
const std::string kInputReplyTypeField = "Reply_type";
const std::string kInputReplyResultField = "result";
const std::string kInputReplyPayloadField = "payload";

// Log entry pv identifier
const std::string kLogEntryId = "LOG";
const std::string kLogEntryType = "sup::logEntryType/v1.0";
// Log entry fields
// const std::string kIndexField already defined
const std::string kSeverityField = "severity";
const std::string kMessageField = "message";

// Basic log entry AnyValue
extern const sup::dto::AnyValue kLogEntryAnyValue;

// Message entry pv identifier
const std::string kMessageEntryId = "MSG";
const std::string kMessageEntryType = "sup::messageEntryType/v1.0";
// Message entry fields
// const std::string kIndexField already defined
// const std::string kMessageField already defined

// Basic message entry AnyValue
extern const sup::dto::AnyValue kMessageEntryAnyValue;

// Output value entry pv identifier
const std::string kOutputValueEntryId = "OUT";
const std::string kOutputValueEntryType = "sup::outputValueEntryType/v1.0";
// Output value entry fields
// const std::string kIndexField already defined
const std::string kDescriptionField = "description";
const std::string kValueField = "value";

// Basic output value entry AnyValue
extern const sup::dto::AnyValue kOutputValueEntryAnyValue;

// Next instructions pv identifier
const std::string kNextInstructionsId = "NEXT_INSTR";
const std::string kNextInstructionsType = "sup::nextInstructionsType/v1.0";

// Basic AnyValue for next instruction indices
extern const sup::dto::AnyValue kNextInstructionsAnyValue;

// Job state postfix:
const std::string kJobStateId = "STATE";
// Job state type name and fields:
const std::string kJobStateType = "sup::jobState/v1.0";
const std::string kJobStateField = "state";

// Basic job state AnyValue
extern const sup::dto::AnyValue kJobStateAnyValue;

// Automation servers will report the following type and version:
const std::string kAutomationInfoServerProtocolServerType = "SUP::AutomationInfoServerProtocol";
const std::string kAutomationInfoServerProtocolServerVersion = "1.0";
const std::string kAutomationControlServerProtocolServerType = "SUP::AutomationControlServerProtocol";
const std::string kAutomationControlServerProtocolServerVersion = "1.0";

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

// Input request servers will report the following type and version:
const std::string kAutomationInputRequestServerType = "SUP::AutoInputServerProtocol";
const std::string kAutomationInputRequestServerVersion = "1.0";

// Supported function names for input request servers:
const std::string KSetReplyFunctionName = "SetReply";

// Field names used for the supported functions of input request servers:
const std::string kUserReplyValueFieldName = "value";

enum class ValueNameType
{
  kUnknown = 0,
  kInstruction,
  kVariable,
  kLogEntry,
  kMessageEntry,
  kOutputValueEntry,
  kJobStatus,
  kNextInstructions
};

struct ValueNameInfo
{
  ValueNameType val_type;
  sup::dto::uint32 idx;
};

// Application specific protocol results:
/**
 * @brief The requested function is not supported.
*/
extern const sup::protocol::ProtocolResult NotSupported;
/**
 * @brief The requested job is unknown.
*/
extern const sup::protocol::ProtocolResult UnknownJob;
/**
 * @brief The requested instruction is unknown.
*/
extern const sup::protocol::ProtocolResult UnknownInstruction;
/**
 * @brief The requested job is unknown.
*/
extern const sup::protocol::ProtocolResult UnknownJobCommand;
/**
 * @brief The client reply to user input is refused.
*/
extern const sup::protocol::ProtocolResult ClientReplyRefused;


/**
 * @brief Create a name for the server that handles job control.
 *
 * @param server_name Main server name that is used for handling information requests about jobs.
 * @return Name for the control server.
 */
std::string GetControlServerName(const std::string& server_name);

/**
 * @brief Convert the protocol result to a human-readable string.
 *
 * @param result a protocol result object.
 * @return The string representation of the result.
 *
 * @details If the provided protocol result is generic (i.e. defined in sup-protocol), this function
 * will call the generic function sup::protocol::ProtocolResultToString.
 */
std::string AutomationServerResultToString(const sup::protocol::ProtocolResult& result);

/**
 * @brief Create a job specific prefix from the server prefix and the job index.
 *
 * @param server_prefix Main server prefix.
 * @param idx Job index.
 * @return Job specific prefix.
 */
std::string CreateJobPrefix(const std::string& server_prefix, sup::dto::uint32 idx);

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
 * @brief Create a name for the server that handles user input for a job.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @return Name for the input server.
 */
std::string GetInputServerName(const std::string& prefix);

/**
 * @brief Create a PV name for publishing input requests.
 *
 * @param server_name Name of the server associated with the input requests.
 * @return PV name for publishing input requests.
 */
std::string GetInputRequestPVName(const std::string& server_name);

/**
 * @brief Create a PV channel name for the log entries.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @return Name for the log entries.
 */
std::string GetLogEntryName(const std::string& prefix);

/**
 * @brief Create a PV channel name for the message entries.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @return Name for the message entries.
 */
std::string GetMessageEntryName(const std::string& prefix);

/**
 * @brief Create a PV channel name for the output value entries.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @return Name for the output value entries.
 */
std::string GetOutputValueEntryName(const std::string& prefix);

/**
 * @brief Create a PV channel name for notifying the next instruction leaves to be executed.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @return Name for notifying the next instruction leaves to be executed.
 */
std::string GetNextInstructionsName(const std::string& prefix);

/**
 * @brief Create a PV channel name for the job state from a given prefix.
 *
 * @param prefix Prefix that needs to be unique among all running jobs in the network.
 * @return PV channel name for the job state.
 */
std::string GetJobStatePVName(const std::string& prefix);

/**
 * @brief Create an AnyValue representing the given job state.
 *
 * @param state Job state to encode in AnyValue.
 * @return AnyValue representing the given job state.
 */
sup::dto::AnyValue GetJobStateValue(sup::sequencer::JobState state);

/**
 * @brief Pack a variable's value and connected state into a base64 encoded AnyValue.
 *
 * @param value Variable's value.
 * @param connected Variable's connected status.
 *
 * @return Packed and encoded AnyValue.
 */
sup::dto::AnyValue EncodeVariableState(const dto::AnyValue& value, bool connected);

/**
 * @brief Decode the packed and encoded AnyValue state of a variable into its value
 * and connected state. On failure, returns an empty AnyValue and false.
 *
 * @param encoded Encoded AnyValue state.
 *
 * @return Pair of variable value and its connected state.
 */
std::pair<sup::dto::AnyValue, bool> DecodeVariableState(const dto::AnyValue& encoded);

/**
 * @brief Encode the indices of the next instruction leaves into a base64 encoded AnyValue.
 *
 * @param next_indices List of instruction indices.
 *
 * @return Encoded AnyValue.
 */
sup::dto::AnyValue EncodeNextInstructionIndices(const std::vector<sup::dto::uint32>& next_indices);

/**
 * @brief Decode the encoded AnyValue of next instruction indices.
 *
 * @param encoded Encoded AnyValue of next instruction indices.
 *
 * @return Pair of boolean indicating success and the decoded list of indices (if successful).
 */
std::pair<bool, std::vector<sup::dto::uint32>> DecodeNextInstructionIndices(
  const dto::AnyValue& encoded);

/**
 * @brief Parse a given AnyValue name into its type (instruction, variable, etc.) and optional
 * index.
 *
 * @param val_name AnyValue name.
 * @return Parsed information. Type kUnknown is returned if the name could not be correctly parsed.
 */
ValueNameInfo ParseValueName(const std::string& val_name);

/**
 * @brief Extract the job index from the given input AnyValue. This is used in RPC server protocols.
 *
 * @param input AnyValue passed as input to a protocol server.
 * @param n_jobs Total number of jobs (to provide bounds for the answer).
 * @param idx Output parameter that will hold the parsed job index.
 * @return ProtocolResult indicating success or failure conditions.
 */
sup::protocol::ProtocolResult ExtractJobIndex(
  const sup::dto::AnyValue& input, sup::dto::uint32 n_jobs, sup::dto::uint32& idx);

/**
 * @brief Extract the instruction index from the given input AnyValue. This is used in RPC server
 * protocols.
 *
 * @param input AnyValue passed as input to a protocol server.
 * @param n_instr Total number of instructions (to provide bounds for the answer).
 * @param idx Output parameter that will hold the parsed instruction index.
 * @return ProtocolResult indicating success or failure conditions.
 */
sup::protocol::ProtocolResult ExtractInstructionIndex(
  const sup::dto::AnyValue& input, sup::dto::uint32 n_instr, sup::dto::uint32& idx);

/**
 * @brief This encoding can be used for all server AnyValues to enforce them to have the same type.
 * The encoded AnyValue will be a struct with two string members (one for the encoding name and
 * the other for the base64 encoded anyvalue).
 *
 * @param value AnyValue to encode.
 * @return Encoded AnyValue.
 */
sup::dto::AnyValue Base64EncodeAnyValue(const sup::dto::AnyValue& value);

/**
 * @brief Decode a base64 encoded AnyValue. See also `Base64EncodeAnyValue`.
 *
 * @param value AnyValue to decode.
 * @return Boolean indicating success of the decoding operation and the decoded AnyValue
 * (if success).
 */
std::pair<bool, sup::dto::AnyValue> Base64DecodeAnyValue(const sup::dto::AnyValue& value);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SUP_AUTO_PROTOCOL_H_
