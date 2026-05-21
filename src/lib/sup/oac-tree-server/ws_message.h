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

#include <string>

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

class WsMessage
{
public:
  WsMessage();
  WsMessage(const WsMessage&) = default;
  WsMessage(WsMessage&&) = default;
  WsMessage& operator=(const WsMessage&) = default;
  WsMessage& operator=(WsMessage&&) = default;
  virtual ~WsMessage();
};

class UpdateMessage : public WsMessage
{
public:
  explicit UpdateMessage(UpdateMessageType type);
  ~UpdateMessage() override;

  UpdateMessageType GetType() const;

private:
  UpdateMessageType m_type;
};

class JobStateUpdateMessage : public UpdateMessage
{
public:
  explicit JobStateUpdateMessage(sup::oac_tree::JobState state);

  sup::oac_tree::JobState GetState() const;

private:
  sup::oac_tree::JobState m_state;
};

class BreakpointInstructionUpdateMessage : public UpdateMessage
{
public:
  explicit BreakpointInstructionUpdateMessage(sup::dto::uint32 instruction_id);

  sup::dto::uint32 GetInstructionId() const;

private:
  sup::dto::uint32 m_instruction_id;
};

class InstructionStateUpdateMessage : public UpdateMessage
{
public:
  InstructionStateUpdateMessage(sup::dto::uint32 instruction_id, bool breakpoint,
                                sup::oac_tree::InstructionState state);

  sup::dto::uint32 GetInstructionId() const;
  bool IsBreakpoint() const;
  sup::oac_tree::InstructionState GetState() const;

private:
  sup::dto::uint32 m_instruction_id;
  bool m_breakpoint;
  sup::oac_tree::InstructionState m_state;
};

class VariableStateUpdateMessage : public UpdateMessage
{
public:
  VariableStateUpdateMessage(sup::dto::uint32 variable_id, bool connected,
                             const sup::dto::AnyValue& value);

  sup::dto::uint32 GetVariableId() const;
  bool IsConnected() const;
  const sup::dto::AnyValue& GetValue() const;

private:
  sup::dto::uint32 m_variable_id;
  bool m_connected;
  sup::dto::AnyValue m_value;
};

class PutValueUpdateMessage : public UpdateMessage
{
public:
  PutValueUpdateMessage(const std::string& description, const sup::dto::AnyValue& value);

  const std::string& GetDescription() const;
  const sup::dto::AnyValue& GetValue() const;

private:
  std::string m_description;
  sup::dto::AnyValue m_value;
};

class TextMessageUpdateMessage : public UpdateMessage
{
public:
  explicit TextMessageUpdateMessage(const std::string& message);

  const std::string& GetMessage() const;

private:
  std::string m_message;
};

class LogUpdateMessage : public UpdateMessage
{
public:
  LogUpdateMessage(sup::dto::int32 severity, const std::string& message);

  sup::dto::int32 GetSeverity() const;
  const std::string& GetMessage() const;

private:
  sup::dto::int32 m_severity;
  std::string m_message;
};

class ClientRequestMessage : public WsMessage
{
public:
  explicit ClientRequestMessage(ClientMessageType type);
  ~ClientRequestMessage() override;

  ClientMessageType GetType() const;

private:
  ClientMessageType m_type;
};

class GetNumberOfJobsRequestMessage : public ClientRequestMessage
{
public:
  GetNumberOfJobsRequestMessage();
};

class GetJobInfoRequestMessage : public ClientRequestMessage
{
public:
  explicit GetJobInfoRequestMessage(sup::dto::uint32 job_index);

  sup::dto::uint32 GetJobIndex() const;

private:
  sup::dto::uint32 m_job_index;
};

class EditBreakpointRequestMessage : public ClientRequestMessage
{
public:
  EditBreakpointRequestMessage(sup::dto::uint32 job_index, sup::dto::uint32 instruction_id,
                               bool active);

  sup::dto::uint32 GetJobIndex() const;
  sup::dto::uint32 GetInstructionId() const;
  bool IsActive() const;

private:
  sup::dto::uint32 m_job_index;
  sup::dto::uint32 m_instruction_id;
  bool m_active;
};

class SendJobCommandRequestMessage : public ClientRequestMessage
{
public:
  SendJobCommandRequestMessage(sup::dto::uint32 job_index, sup::oac_tree::JobCommand command);

  sup::dto::uint32 GetJobIndex() const;
  sup::oac_tree::JobCommand GetCommand() const;

private:
  sup::dto::uint32 m_job_index;
  sup::oac_tree::JobCommand m_command;
};

class ClientReplyMessage : public WsMessage
{
public:
  explicit ClientReplyMessage(ClientMessageType type);
  ~ClientReplyMessage() override;

  ClientMessageType GetType() const;

private:
  ClientMessageType m_type;
};

class GetNumberOfJobsReplyMessage : public ClientReplyMessage
{
public:
  explicit GetNumberOfJobsReplyMessage(sup::dto::uint32 number_of_jobs);

  sup::dto::uint32 GetNumberOfJobs() const;

private:
  sup::dto::uint32 m_number_of_jobs;
};

class GetJobInfoReplyMessage : public ClientReplyMessage
{
public:
  explicit GetJobInfoReplyMessage(const sup::oac_tree::JobInfo& job_info);

  const sup::oac_tree::JobInfo& GetJobInfo() const;

private:
  sup::oac_tree::JobInfo m_job_info;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_BASE_WS_MESSAGE_H_
