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

AutomationJobInterface::AutomationJobInterface(const std::string& prefix, const sequencer::Procedure& proc,
                                       AnyValueServerInterface& server_interface)
  : m_job_value_mapper{prefix, proc}
  , m_instr_tree_cache{}
  , m_instr_tree_anyvalue{}
  , m_server_interface{server_interface}
{
  auto value_set = GetValueSet();
  m_server_interface.ServeAnyValues(value_set);
}

AutomationJobInterface::~AutomationJobInterface() = default;

void AutomationJobInterface::InitializeInstructionTree(const sequencer::Instruction* root)
{
  m_instr_tree_cache.InitializeCache(root);
  m_instr_tree_anyvalue = m_instr_tree_cache.GetInitialInstructionTreeAnyValue();
  auto instr_tree_name = m_job_value_mapper.GetInstructionTreeName();
  AnyValueServerInterface::NameAnyValueSet values_to_serve;
  values_to_serve.emplace_back(instr_tree_name, m_instr_tree_anyvalue);
  m_server_interface.ServeAnyValues(values_to_serve);
}

void AutomationJobInterface::UpdateInstructionStatus(const sequencer::Instruction* instruction)
{
  auto name = m_job_value_mapper.GetInstructionTreeName();
  auto status = instruction->GetStatus();
  auto path = m_instr_tree_cache.FindInstructionPath(instruction);
  auto& instr_node = path.empty() ? m_instr_tree_anyvalue
                                  : m_instr_tree_anyvalue[path];
  instr_node[kExecStatusField] = static_cast<sup::dto::uint16>(status);
  m_server_interface.UpdateAnyValue(name, m_instr_tree_anyvalue);
}

void AutomationJobInterface::VariableUpdated(const std::string& name, const sup::dto::AnyValue& value,
                                          bool connected)
{
  auto var_value_name = m_job_value_mapper.GetVariableValueName(name);
  auto var_info = EncodeVariableInfo(name, value, connected);
  m_server_interface.UpdateAnyValue(var_value_name, var_info);
}

bool AutomationJobInterface::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{
  (void)value;
  (void)description;
  return true;
}

bool AutomationJobInterface::GetUserValue(sup::dto::AnyValue& value, const std::string& description)
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
  auto name = m_job_value_mapper.GetInstructionTreeName();
  auto path = m_instr_tree_cache.FindInstructionPath(instruction);
  auto& instr_node = path.empty() ? m_instr_tree_anyvalue
                                  : m_instr_tree_anyvalue[path];
  instr_node[kBreakpointField] = breakpoint_set;
  m_server_interface.UpdateAnyValue(name, m_instr_tree_anyvalue);
}

void AutomationJobInterface::OnProcedureTick(const sequencer::Procedure& proc) noexcept
{
  // TODO: update next instruction leaves
  (void)proc;
}

AnyValueServerInterface::NameAnyValueSet AutomationJobInterface::GetValueSet() const
{
  AnyValueServerInterface::NameAnyValueSet result;
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

}  // namespace auto_server

}  // namespace sup
