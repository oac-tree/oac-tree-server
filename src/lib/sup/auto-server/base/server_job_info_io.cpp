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

#include <sup/auto-server/server_job_info_io.h>

#include <sup/auto-server/anyvalue_input_request.h>
#include <sup/auto-server/output_entries.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/dto/anyvalue_helper.h>

namespace sup
{
namespace auto_server
{
ServerJobInfoIO::ServerJobInfoIO(const std::string& job_prefix, sup::dto::uint32 n_vars,
                                 IAnyValueManager& av_manager)
  : m_job_prefix{job_prefix}
  , m_n_vars{n_vars}
  , m_av_manager{av_manager}
{
  auto value_set = GetInitialValueSet(m_job_prefix, m_n_vars);
  m_av_manager.AddAnyValues(value_set);
  auto input_server_name = GetInputServerName(m_job_prefix);
  m_av_manager.AddInputServer(input_server_name);
}

ServerJobInfoIO::~ServerJobInfoIO() = default;

void ServerJobInfoIO::InitNumberOfInstructions(sup::dto::uint32 n_instr)
{
  auto instr_value_set = GetInstructionValueSet(m_job_prefix, n_instr);
  m_av_manager.AddAnyValues(instr_value_set);
}

void ServerJobInfoIO::InstructionStateUpdated(sup::dto::uint32 instr_idx, InstructionState state)
{
  auto instr_val_name = GetInstructionPVName(m_job_prefix, instr_idx);
  auto instr_state_av = ToAnyValue(state);
  m_av_manager.UpdateAnyValue(instr_val_name, instr_state_av);
}

void ServerJobInfoIO::VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                                      bool connected)
{
  auto var_val_name = GetVariablePVName(m_job_prefix, var_idx);
  auto var_info = EncodeVariableState(value, connected);
  m_av_manager.UpdateAnyValue(var_val_name, var_info);
}

bool ServerJobInfoIO::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{
  // TODO: get index
  OutputValueEntry out_val{ 0, description, value };
  return true;
}

bool ServerJobInfoIO::GetUserValue(sup::dto::AnyValue& value, const std::string& description)
{
  auto input_request = CreateUserValueRequest(value, description);
  auto input_server_name = GetInputServerName(m_job_prefix);
  auto response = m_av_manager.GetUserInput(input_server_name, input_request);
  auto parsed_reply = ParseUserValueReply(response);
  if (!parsed_reply.first)
  {
    return false;
  }
  // Use TryAssign, since it is up to concrete Instructions to handle possible type mismatches:
  if (!sup::dto::TryAssign(value, parsed_reply.second))
  {
    return false;
  }
  return true;
}

int ServerJobInfoIO::GetUserChoice(const std::vector<std::string>& options,
                                   const sup::dto::AnyValue& metadata)
{
  auto input_request = CreateUserChoiceRequest(options, metadata);
  auto input_server_name = GetInputServerName(m_job_prefix);
  auto response = m_av_manager.GetUserInput(input_server_name, input_request);
  auto parsed_reply = ParseUserChoiceReply(response);
  if (!parsed_reply.first)
  {
    return -1;
  }
  return parsed_reply.second;
}

void ServerJobInfoIO::Message(const std::string& message)
{
  // TODO: get index
  MessageEntry msg_val{ 0, message };
}

void ServerJobInfoIO::Log(int severity, const std::string& message)
{
  // TODO: get index
  LogEntry log_val{ 0, severity, message };
}

void ServerJobInfoIO::JobStateUpdated(sup::sequencer::JobState state)
{
  auto job_state_name = GetJobStatePVName(m_job_prefix);
  auto job_state_value = GetJobStateValue(state);
  m_av_manager.UpdateAnyValue(job_state_name, job_state_value);
}

}  // namespace auto_server

}  // namespace sup
