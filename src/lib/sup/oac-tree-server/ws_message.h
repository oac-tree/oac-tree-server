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

const char* kUpdate = "update";
const char* kState = "state";
const char* kId = "id";
const char* kBreakpoint = "breakpoint";
const char* kConnected = "connected";
const char* kValue = "value";
const char* kType = "type";
const char* kDescription = "description";
const char* kMessage = "message";
const char* kSeverity = "severity";
const char* kRequest = "request";
const char* kJobIndex = "job_index";
const char* kInstructionId = "instr_id";
const char* kActive = "active";
const char* kCommand = "command";
const char* kNumberOfJobs = "n_jobs";
const char* kJobInfo = "job_info";
const char* kEmptyJson = "";

}  // namespace ws_message_json_keys

std::string CreateJobStateUpdateMessage(sup::oac_tree::JobState state);
std::string CreateBreakpointInstructionUpdateMessage(sup::dto::uint32 instruction_id);
std::string CreateInstructionStateUpdateMessage(sup::dto::uint32 instruction_id,
                                                bool breakpoint,
                                                sup::oac_tree::InstructionState state);
std::string CreateVariableStateUpdateMessage(sup::dto::uint32 variable_id,
                                             bool connected,
                                             const sup::dto::AnyValue& value);
std::string CreatePutValueUpdateMessage(const std::string& description,
                                        const sup::dto::AnyValue& value);
std::string CreateTextMessageUpdateMessage(const std::string& message);
std::string CreateLogUpdateMessage(sup::dto::int32 severity,
                                   const std::string& message);

std::string CreateGetNumberOfJobsRequestMessage();
std::string CreateGetJobInfoRequestMessage(sup::dto::uint32 job_index);
std::string CreateEditBreakpointRequestMessage(sup::dto::uint32 job_index,
                                               sup::dto::uint32 instruction_id,
                                               bool active);
std::string CreateSendJobCommandRequestMessage(sup::dto::uint32 job_index,
                                               sup::oac_tree::JobCommand command);

std::string CreateGetNumberOfJobsReplyMessage(sup::dto::uint32 number_of_jobs);
std::string CreateGetJobInfoReplyMessage(const sup::oac_tree::JobInfo& job_info);

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_BASE_WS_MESSAGE_H_
