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

#include <sup/auto-server/job_value_mapper.h>

#include "variable_map.h"

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/instruction_map.h>
#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{

JobValueMapper::JobValueMapper(const std::string& prefix, const sequencer::Procedure& proc)
  : m_prefix{prefix}
  , m_variable_map{new VariableMap{proc.GetWorkspace()}}
  , m_instruction_map{}
{}

JobValueMapper::~JobValueMapper() = default;

void JobValueMapper::InitializeInstructionTree(const sequencer::Instruction* root)
{
  m_instruction_map.reset(new InstructionMap{root});
}

std::string JobValueMapper::GetJobStateName() const
{
  return GetJobStatePVName(m_prefix);
}

const InstructionMap& JobValueMapper::GetInstructionMap() const
{
  if (!m_instruction_map)
  {
    const std::string error = "JobValueMapper::GetInstructionMap(): object was not "
      "correctly initialized with a root instruction";
    throw InvalidOperationException(error);
  }
  return *m_instruction_map;
}

sup::dto::uint32 JobValueMapper::GetNumberOfInstructions() const
{
  if (!m_instruction_map)
  {
    return 0;
  }
  return m_instruction_map->GetNumberOfInstructions();
}

sup::dto::uint32 JobValueMapper::GetInstructionIndex(const sequencer::Instruction* instr) const
{
  if (!m_instruction_map)
  {
    const std::string error = "JobValueMapper::GetInstructionIndex(): object was not "
      "correctly initialized with a root instruction";
    throw InvalidOperationException(error);
  }
  return m_instruction_map->FindInstructionIndex(instr);
}


std::string JobValueMapper::GetInstructionValueName(const sequencer::Instruction* instr) const
{
  if (!m_instruction_map)
  {
    const std::string error = "JobValueMapper::GetInstructionValueName(): object was not "
      "correctly initialized with a root instruction";
    throw InvalidOperationException(error);
  }
  auto idx = m_instruction_map->FindInstructionIndex(instr);
  return GetInstructionPVName(m_prefix, idx);
}

std::vector<const sequencer::Instruction*> JobValueMapper::GetOrderedInstructions() const
{
  if (!m_instruction_map)
  {
    const std::string error = "JobValueMapper::GetOrderedInstructions(): object was not "
      "correctly initialized with a root instruction";
    throw InvalidOperationException(error);
  }
  return GetReverseMap(m_instruction_map->GetInstructionMapping());
}

std::vector<std::string> JobValueMapper::GetInstructionValueNames() const
{
  std::vector<std::string> result;
  if (m_instruction_map)
  {
    for (sup::dto::uint32 idx = 0; idx < m_instruction_map->GetNumberOfInstructions(); ++idx)
    {
      result.push_back(GetInstructionPVName(m_prefix, idx));
    }
  }
  return result;
}

sup::dto::uint32 JobValueMapper::GetNumberOfVariables() const
{
  return m_variable_map->GetNumberOfVariables();
}

sup::dto::uint32 JobValueMapper::GetVariableIndex(const std::string& var_name) const
{
  return m_variable_map->FindVariableIndex(var_name);
}

std::string JobValueMapper::GetVariableValueName(const std::string& var_name) const
{
  auto idx = m_variable_map->FindVariableIndex(var_name);
  return GetVariablePVName(m_prefix, idx);
}

std::vector<std::string> JobValueMapper::GetVariableValueNames() const
{
  std::vector<std::string> result;
  for (sup::dto::uint32 idx = 0; idx < m_variable_map->GetNumberOfVariables(); ++idx)
  {
    result.push_back(GetVariablePVName(m_prefix, idx));
  }
  return result;
}

}  // namespace auto_server

}  // namespace sup
