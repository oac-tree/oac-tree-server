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

#include <sup/auto-server/automation_job_interface.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/auto-server/base/instruction_tree_utils.h>

#include <sup/sequencer/instruction.h>

namespace sup
{
namespace auto_server
{

AutomationJobInterface::AutomationJobInterface(const std::string& prefix,
                                               const sequencer::Procedure& proc,
                                               AnyValueManagerInterface& server_interface)
  : m_job_value_mapper{prefix, proc}
  , m_instr_states{}
  , m_server_interface{server_interface}
{
  auto value_set = GetInitialValueSet();
  m_server_interface.AddAnyValues(value_set);
}

AutomationJobInterface::~AutomationJobInterface() = default;

void AutomationJobInterface::InitializeInstructionTree(const sequencer::Instruction* root)
{
  m_job_value_mapper.InitializeInstructionTree(root);
  m_instr_states = std::vector<sup::dto::AnyValue>(m_job_value_mapper.GetNumberOfInstructions(),
                                                   kInstructionAnyValue);
  auto instr_value_set = GetInstructionValueSet();
  m_server_interface.AddAnyValues(instr_value_set);
}

sup::dto::AnyValue AutomationJobInterface::GetInstructionTreeInfo(
  const sequencer::Instruction* root) const
{
  return utils::BuildInstructionTreeInfo(root, m_job_value_mapper);
}

std::vector<const sequencer::Instruction*> AutomationJobInterface::GetOrderedInstructions() const
{
  return m_job_value_mapper.GetOrderedInstructions();
}

void AutomationJobInterface::UpdateInstructionStatus(const sequencer::Instruction* instruction)
{
  auto name = m_job_value_mapper.GetInstructionValueName(instruction);
  auto status = instruction->GetStatus();
  auto instr_idx = m_job_value_mapper.GetInstructionIndex(instruction);
  auto& instr_state = m_instr_states[instr_idx];
  instr_state[kExecStatusField] = static_cast<sup::dto::uint16>(status);
  m_server_interface.UpdateAnyValue(name, instr_state);
}

void AutomationJobInterface::VariableUpdated(const std::string& name,
                                             const sup::dto::AnyValue& value,
                                             bool connected)
{
  auto var_value_name = m_job_value_mapper.GetVariableValueName(name);
  auto var_info = EncodeVariableInfo(value, connected);
  m_server_interface.UpdateAnyValue(var_value_name, var_info);
}

bool AutomationJobInterface::PutValue(const sup::dto::AnyValue& value,
                                      const std::string& description)
{
  (void)value;
  (void)description;
  return true;
}

bool AutomationJobInterface::GetUserValue(sup::dto::AnyValue& value,
                                          const std::string& description)
{
  (void)value;
  (void)description;
  return false;
}

int AutomationJobInterface::GetUserChoice(const std::vector<std::string>& options,
                                          const sup::dto::AnyValue& metadata)
{
  (void)options;
  (void)metadata;
  return -1;
}

void AutomationJobInterface::Message(const std::string& message)
{
  (void)message;
}

void AutomationJobInterface::Log(int severity, const std::string& message)
{
  (void)severity;
  (void)message;
}

void AutomationJobInterface::OnStateChange(sequencer::JobState state) noexcept
{
  auto name = m_job_value_mapper.GetJobStateName();
  auto value = GetJobStateValue(state);
  m_server_interface.UpdateAnyValue(name, value);
}

void AutomationJobInterface::OnBreakpointChange(const sequencer::Instruction* instruction,
                                                bool breakpoint_set) noexcept
{
  auto name = m_job_value_mapper.GetInstructionValueName(instruction);
  auto instr_idx = m_job_value_mapper.GetInstructionIndex(instruction);
  auto& instr_state = m_instr_states[instr_idx];
  instr_state[kBreakpointField] = breakpoint_set;
  m_server_interface.UpdateAnyValue(name, instr_state);
}

void AutomationJobInterface::OnProcedureTick(const sequencer::Procedure& proc) noexcept
{
  // TODO: update next instruction leaves
  (void)proc;
}

AnyValueManagerInterface::NameAnyValueSet AutomationJobInterface::GetInitialValueSet() const
{
  AnyValueManagerInterface::NameAnyValueSet result;
  auto job_value_name = m_job_value_mapper.GetJobStateName();
  auto job_value = GetJobStateValue(sequencer::JobState::kInitial);
  result.emplace_back(job_value_name, job_value);
  auto variable_names = m_job_value_mapper.GetVariableValueNames();
  for (const auto& name : variable_names)
  {
    result.emplace_back(name, kVariableAnyValue);
  }
  return result;
}

AnyValueManagerInterface::NameAnyValueSet AutomationJobInterface::GetInstructionValueSet() const
{
  AnyValueManagerInterface::NameAnyValueSet result;
  auto instr_names = m_job_value_mapper.GetInstructionValueNames();
  for (const auto& name : instr_names)
  {
    result.emplace_back(name, kInstructionAnyValue);
  }
  return result;
}

}  // namespace auto_server

}  // namespace sup
