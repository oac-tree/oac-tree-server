/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : oac-tree server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_OAC_TREE_SERVER_BASE_WS_MESSAGE_H_
#define SUP_OAC_TREE_SERVER_BASE_WS_MESSAGE_H_

#include <sup/dto/anyvalue.h>
#include <sup/dto/basic_scalar_types.h>
#include <sup/oac-tree/i_job_info_io.h>
#include <sup/oac-tree/job_commands.h>
#include <sup/oac-tree/job_info.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace sup
{
namespace oac_tree_server
{

enum class UpdateMessageType : sup::dto::uint32
{
  kJobState = 0,
  kBreakPoint,
  kInstructionState,
  kVariableState,
  kPutValue,
  kMessage,
  kLog,
  kGetUserValue,
  kGetUserChoice,
};

enum class ClientMessageType : sup::dto::uint32
{
  kGetNumberOfJobs = 0,
  kGetJobInfo,
  kEditBreakPoint,
  kSendCommand,
};

namespace ws_message_json_keys
{

constexpr auto kUpdate = "update";
constexpr auto kState = "state";
constexpr auto kId = "id";
constexpr auto kBreakpoint = "breakpoint";
constexpr auto kConnected = "connected";
constexpr auto kValue = "value";
constexpr auto kType = "type";
constexpr auto kDescription = "description";
constexpr auto kMessage = "message";
constexpr auto kSeverity = "severity";
constexpr auto kRequest = "request";
constexpr auto kRequestId = "req_id";
constexpr auto kJobIndex = "job_index";
constexpr auto kInstructionId = "instr_id";
constexpr auto kActive = "active";
constexpr auto kCommand = "command";
constexpr auto kNumberOfJobs = "n_jobs";
constexpr auto kChoices = "choices";
constexpr auto kChoice = "choice";
constexpr auto kOk = "ok";
constexpr auto kEmptyJson = "";

}  // namespace ws_message_json_keys

namespace endpoints
{
constexpr auto kJobManagerEndpoint = "/info";
constexpr auto kJobEndpointPrefix = "/job/";
}

std::string CreateServerPrefix(const std::string& host_name, unsigned short port);
std::string CreateInfoEndpoint(const std::string& server_prefix);
std::string CreateJobEndpoint(const std::string& server_prefix, sup::dto::uint32 job_idx);

// Factory functions for creating JSON messages for job specific updates from server to client.
std::string CreateJobStateUpdateMessage(sup::oac_tree::JobState state);
std::string CreateBreakpointInstructionUpdateMessage(sup::dto::uint32 instruction_id);
std::string CreateInstructionStateUpdateMessage(sup::dto::uint32 instruction_id,
                                                sup::oac_tree::InstructionState state);
std::string CreateVariableStateUpdateMessage(sup::dto::uint32 variable_id,
                                             bool connected,
                                             const sup::dto::AnyValue& value);
std::string CreatePutValueUpdateMessage(const std::string& description,
                                        const sup::dto::AnyValue& value);
std::string CreateTextMessageUpdateMessage(const std::string& message);
std::string CreateLogUpdateMessage(sup::dto::int32 severity,
                                   const std::string& message);
std::string CreateGetUserValueUpdateMessage(sup::dto::uint64 request_id,
                                            const std::string& description,
                                            const sup::dto::AnyValue& value);
std::string CreateGetUserChoiceUpdateMessage(sup::dto::uint64 request_id,
                                             const std::vector<std::string>& choices,
                                             const sup::dto::AnyValue& metadata);

// Factory functions for creating JSON messages for global requests from client to server.
std::string CreateGetNumberOfJobsRequestMessage(sup::dto::uint64 request_id);
std::string CreateGetJobInfoRequestMessage(sup::dto::uint64 request_id, sup::dto::uint32 job_index);
std::string CreateEditBreakpointRequestMessage(sup::dto::uint32 job_index,
                                               sup::dto::uint32 instruction_id,
                                               bool active);
std::string CreateSendJobCommandRequestMessage(sup::dto::uint32 job_index,
                                               sup::oac_tree::JobCommand command);

// Factory functions for creating JSON messages for global replies from server to client.
std::string CreateGetNumberOfJobsReplyMessage(sup::dto::uint64 request_id,
                                              sup::dto::uint32 number_of_jobs);
std::string CreateGetJobInfoReplyMessage(sup::dto::uint64 request_id, sup::dto::uint32 job_index,
                                         const sup::oac_tree::JobInfo& job_info);

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_BASE_WS_MESSAGE_H_
