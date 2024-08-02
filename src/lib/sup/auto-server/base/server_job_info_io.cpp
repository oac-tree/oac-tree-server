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

namespace
{
using namespace sup::auto_server;
IAnyValueManager::NameAnyValueSet GetInstructionValueSet(const std::string& job_prefix,
                                                         sup::dto::uint32 n_instr);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{
ServerJobInfoIO::ServerJobInfoIO(const std::string& job_prefix, sup::dto::uint32 n_vars,
                                 IAnyValueManager& av_manager)
  : m_job_prefix{job_prefix}
  , m_n_vars{n_vars}
  , m_instr_states{}
  , m_av_manager{av_manager}
{
  auto value_set = GetInitialValueSet();
  m_av_manager.AddAnyValues(value_set);
}

ServerJobInfoIO::~ServerJobInfoIO() = default;

void ServerJobInfoIO::InitNumberOfInstructions(sup::dto::uint32 n_instr)
{
  m_instr_states = std::vector<sup::dto::AnyValue>(n_instr, kInstructionAnyValue);
  auto instr_value_set = GetInstructionValueSet(m_job_prefix, n_instr);
  m_av_manager.AddAnyValues(instr_value_set);
}

void ServerJobInfoIO::UpdateInstructionStatus(sup::dto::uint32 instr_idx,
                                              sup::sequencer::ExecutionStatus status)
{
  if (instr_idx >= m_instr_states.size())
  {
    return;
  }
  auto instr_val_name = GetInstructionPVName(m_job_prefix, instr_idx);
  auto& instr_state = m_instr_states[instr_idx];
  instr_state[kExecStatusField] = static_cast<sup::dto::uint16>(status);
  m_av_manager.UpdateAnyValue(instr_val_name, instr_state);
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
  m_av_manager.UpdateAnyValue(job_state_name, job_state_value);
}

void ServerJobInfoIO::OnBreakpointChange(sup::dto::uint32 instr_idx, bool breakpoint_set)
{
  if (instr_idx >= m_instr_states.size())
  {
    return;
  }
  auto instr_val_name = GetInstructionPVName(m_job_prefix, instr_idx);
  auto& instr_state = m_instr_states[instr_idx];
  instr_state[kBreakpointField] = breakpoint_set;
  m_av_manager.UpdateAnyValue(instr_val_name, instr_state);
}

IAnyValueManager::NameAnyValueSet ServerJobInfoIO::GetInitialValueSet() const
{
  IAnyValueManager::NameAnyValueSet result;
  auto job_value_name = GetJobStatePVName(m_job_prefix);
  auto job_value = GetJobStateValue(sequencer::JobState::kInitial);
  result.emplace_back(job_value_name, job_value);
  for (sup::dto::uint32 var_idx = 0; var_idx < m_n_vars; ++var_idx)
  {
    const auto name = GetVariablePVName(m_job_prefix, var_idx);
    result.emplace_back(name, kVariableAnyValue);
  }
  return result;
}

}  // namespace auto_server

}  // namespace sup

namespace
{
IAnyValueManager::NameAnyValueSet GetInstructionValueSet(const std::string& job_prefix,
                                                         sup::dto::uint32 n_instr)
{
  IAnyValueManager::NameAnyValueSet result;
  for (sup::dto::uint32 instr_idx = 0; instr_idx < n_instr; ++instr_idx)
  {
    const auto name = GetInstructionPVName(job_prefix, instr_idx);
    result.emplace_back(name, kInstructionAnyValue);
  }
  return result;
}
}  // unnamed namespace
