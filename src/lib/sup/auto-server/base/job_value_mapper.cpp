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

#include "instruction_map.h"
#include "variable_map.h"

#include <sup/auto-server/exceptions.h>
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

std::string JobValueMapper::GetInstructionTreeName() const
{
  return GetInstructionTreePVName(m_prefix);
}

std::string JobValueMapper::GetJobStateName() const
{
  return GetJobStatePVName(m_prefix);
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
