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

#include <sup/oac-tree-server/ws_message.h>

namespace sup
{
namespace oac_tree_server
{

WsMessage::WsMessage() = default;
WsMessage::~WsMessage() = default;

UpdateMessage::UpdateMessage(UpdateMessageType type)
  : WsMessage{}
  , m_type{type}
{}

UpdateMessage::~UpdateMessage() = default;

UpdateMessageType UpdateMessage::GetType() const
{
  return m_type;
}

JobStateUpdateMessage::JobStateUpdateMessage(sup::oac_tree::JobState state)
  : UpdateMessage{UpdateMessageType::kJobState}
  , m_state{state}
{}

sup::oac_tree::JobState JobStateUpdateMessage::GetState() const
{
  return m_state;
}

BreakpointInstructionUpdateMessage::BreakpointInstructionUpdateMessage(sup::dto::uint32 instruction_id)
  : UpdateMessage{UpdateMessageType::kBreakPoint}
  , m_instruction_id{instruction_id}
{}

sup::dto::uint32 BreakpointInstructionUpdateMessage::GetInstructionId() const
{
  return m_instruction_id;
}

InstructionStateUpdateMessage::InstructionStateUpdateMessage(
    sup::dto::uint32 instruction_id, bool breakpoint, sup::oac_tree::InstructionState state)
  : UpdateMessage{UpdateMessageType::kInstructionState}
  , m_instruction_id{instruction_id}
  , m_breakpoint{breakpoint}
  , m_state{state}
{}

sup::dto::uint32 InstructionStateUpdateMessage::GetInstructionId() const
{
  return m_instruction_id;
}

bool InstructionStateUpdateMessage::IsBreakpoint() const
{
  return m_breakpoint;
}

sup::oac_tree::InstructionState InstructionStateUpdateMessage::GetState() const
{
  return m_state;
}

VariableStateUpdateMessage::VariableStateUpdateMessage(sup::dto::uint32 variable_id,
                                                       bool connected,
                                                       const sup::dto::AnyValue& value)
  : UpdateMessage{UpdateMessageType::kVariableState}
  , m_variable_id{variable_id}
  , m_connected{connected}
  , m_value{value}
{}

sup::dto::uint32 VariableStateUpdateMessage::GetVariableId() const
{
  return m_variable_id;
}

bool VariableStateUpdateMessage::IsConnected() const
{
  return m_connected;
}

const sup::dto::AnyValue& VariableStateUpdateMessage::GetValue() const
{
  return m_value;
}

PutValueUpdateMessage::PutValueUpdateMessage(const std::string& description,
                                             const sup::dto::AnyValue& value)
  : UpdateMessage{UpdateMessageType::kPutValue}
  , m_description{description}
  , m_value{value}
{}

const std::string& PutValueUpdateMessage::GetDescription() const
{
  return m_description;
}

const sup::dto::AnyValue& PutValueUpdateMessage::GetValue() const
{
  return m_value;
}

TextMessageUpdateMessage::TextMessageUpdateMessage(const std::string& message)
  : UpdateMessage{UpdateMessageType::kMessage}
  , m_message{message}
{}

const std::string& TextMessageUpdateMessage::GetMessage() const
{
  return m_message;
}

LogUpdateMessage::LogUpdateMessage(sup::dto::int32 severity, const std::string& message)
  : UpdateMessage{UpdateMessageType::kLog}
  , m_severity{severity}
  , m_message{message}
{}

sup::dto::int32 LogUpdateMessage::GetSeverity() const
{
  return m_severity;
}

const std::string& LogUpdateMessage::GetMessage() const
{
  return m_message;
}

ClientRequestMessage::ClientRequestMessage(ClientMessageType type)
  : WsMessage{}
  , m_type{type}
{}

ClientRequestMessage::~ClientRequestMessage() = default;

ClientMessageType ClientRequestMessage::GetType() const
{
  return m_type;
}

GetNumberOfJobsRequestMessage::GetNumberOfJobsRequestMessage()
  : ClientRequestMessage{ClientMessageType::kGetNumberOfJobs}
{}

GetJobInfoRequestMessage::GetJobInfoRequestMessage(sup::dto::uint32 job_index)
  : ClientRequestMessage{ClientMessageType::kGetJobInfo}
  , m_job_index{job_index}
{}

sup::dto::uint32 GetJobInfoRequestMessage::GetJobIndex() const
{
  return m_job_index;
}

EditBreakpointRequestMessage::EditBreakpointRequestMessage(sup::dto::uint32 job_index,
                                                           sup::dto::uint32 instruction_id,
                                                           bool active)
  : ClientRequestMessage{ClientMessageType::kEditBreakPoint}
  , m_job_index{job_index}
  , m_instruction_id{instruction_id}
  , m_active{active}
{}

sup::dto::uint32 EditBreakpointRequestMessage::GetJobIndex() const
{
  return m_job_index;
}

sup::dto::uint32 EditBreakpointRequestMessage::GetInstructionId() const
{
  return m_instruction_id;
}

bool EditBreakpointRequestMessage::IsActive() const
{
  return m_active;
}

SendJobCommandRequestMessage::SendJobCommandRequestMessage(sup::dto::uint32 job_index,
                                                           sup::oac_tree::JobCommand command)
  : ClientRequestMessage{ClientMessageType::kSendCommand}
  , m_job_index{job_index}
  , m_command{command}
{}

sup::dto::uint32 SendJobCommandRequestMessage::GetJobIndex() const
{
  return m_job_index;
}

sup::oac_tree::JobCommand SendJobCommandRequestMessage::GetCommand() const
{
  return m_command;
}

ClientReplyMessage::ClientReplyMessage(ClientMessageType type)
  : WsMessage{}
  , m_type{type}
{}

ClientReplyMessage::~ClientReplyMessage() = default;

ClientMessageType ClientReplyMessage::GetType() const
{
  return m_type;
}

GetNumberOfJobsReplyMessage::GetNumberOfJobsReplyMessage(sup::dto::uint32 number_of_jobs)
  : ClientReplyMessage{ClientMessageType::kGetNumberOfJobs}
  , m_number_of_jobs{number_of_jobs}
{}

sup::dto::uint32 GetNumberOfJobsReplyMessage::GetNumberOfJobs() const
{
  return m_number_of_jobs;
}

GetJobInfoReplyMessage::GetJobInfoReplyMessage(const sup::oac_tree::JobInfo& job_info)
  : ClientReplyMessage{ClientMessageType::kGetJobInfo}
  , m_job_info{job_info}
{}

const sup::oac_tree::JobInfo& GetJobInfoReplyMessage::GetJobInfo() const
{
  return m_job_info;
}

}  // namespace oac_tree_server

}  // namespace sup
