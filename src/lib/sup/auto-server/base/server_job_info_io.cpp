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

#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{
ServerJobInfoIO::ServerJobInfoIO(const std::string& job_prefix)
  : m_job_prefix{job_prefix}
{}

ServerJobInfoIO::~ServerJobInfoIO() = default;

void ServerJobInfoIO::UpdateInstructionStatus(sup::dto::uint32 instr_idx,
                                              sup::sequencer::ExecutionStatus status)
{
  auto instr_val_name = GetInstructionPVName(m_job_prefix, instr_idx);
  (void)status;
  // update cache with new status and push update
}

void ServerJobInfoIO::VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                                      bool connected)
{
  auto var_val_name = GetVariablePVName(m_job_prefix, var_idx);
  auto var_info = EncodeVariableState(value, connected);
  // push update
}

bool ServerJobInfoIO::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{
  (void)value;
  (void)description;
  return true;
}

bool ServerJobInfoIO::GetUserValue(sup::dto::AnyValue& value, const std::string& description)
{
  (void)value;
  (void)description;
  return false;
}

int ServerJobInfoIO::GetUserChoice(const std::vector<std::string>& options,
                    const sup::dto::AnyValue& metadata)
{
  (void)options;
  (void)metadata;
  return -1;
}

void ServerJobInfoIO::Message(const std::string& message)
{
  (void)message;
}

void ServerJobInfoIO::Log(int severity, const std::string& message)
{
  (void)severity;
  (void)message;
}

void ServerJobInfoIO::OnStateChange(sup::sequencer::JobState state)
{
  auto job_state_name = GetJobStatePVName(m_job_prefix);
  auto job_state_value = GetJobStateValue(state);
  // push update
}

void ServerJobInfoIO::OnBreakpointChange(sup::dto::uint32 instr_idx, bool breakpoint_set)
{
  auto instr_val_name = GetInstructionPVName(m_job_prefix, instr_idx);
  (void)breakpoint_set;
  // update cache with new status and push update
}

}  // namespace auto_server

}  // namespace sup
