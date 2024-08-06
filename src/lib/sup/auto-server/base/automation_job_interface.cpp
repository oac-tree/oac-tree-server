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

#include <sup/auto-server/instruction_tree_utils.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/instruction.h>

namespace sup
{
namespace auto_server
{

AutomationJobInterface::AutomationJobInterface(const std::string& prefix,
                                               const sequencer::Procedure& proc,
                                               IJobInfoIO& job_info_io)
  : m_job_value_mapper{prefix, proc}
  , m_instr_states{}
  , m_job_info_io{job_info_io}
{}

AutomationJobInterface::~AutomationJobInterface() = default;

void AutomationJobInterface::InitializeInstructionTree(const sequencer::Instruction* root)
{
  m_job_value_mapper.InitializeInstructionTree(root);
  m_job_info_io.InitNumberOfInstructions(m_job_value_mapper.GetNumberOfInstructions());
}

const InstructionMap& AutomationJobInterface::GetInstructionMap() const
{
  return m_job_value_mapper.GetInstructionMap();
}

std::vector<const sequencer::Instruction*> AutomationJobInterface::GetOrderedInstructions() const
{
  return m_job_value_mapper.GetOrderedInstructions();
}

void AutomationJobInterface::UpdateInstructionStatus(const sequencer::Instruction* instruction)
{
  auto instr_idx = m_job_value_mapper.GetInstructionIndex(instruction);
  auto status = instruction->GetStatus();
  m_job_info_io.UpdateInstructionStatus(instr_idx, status);
}

void AutomationJobInterface::VariableUpdated(const std::string& name,
                                             const sup::dto::AnyValue& value,
                                             bool connected)
{
  auto var_idx = m_job_value_mapper.GetVariableIndex(name);
  m_job_info_io.VariableUpdated(var_idx, value, connected);
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
  m_job_info_io.OnStateChange(state);
}

void AutomationJobInterface::OnBreakpointChange(const sequencer::Instruction* instruction,
                                                bool breakpoint_set) noexcept
{
  auto instr_idx = m_job_value_mapper.GetInstructionIndex(instruction);
  m_job_info_io.OnBreakpointChange(instr_idx, breakpoint_set);
}

void AutomationJobInterface::OnProcedureTick(const sequencer::Procedure& proc) noexcept
{
  // TODO: update next instruction leaves
  (void)proc;
}

}  // namespace auto_server

}  // namespace sup
