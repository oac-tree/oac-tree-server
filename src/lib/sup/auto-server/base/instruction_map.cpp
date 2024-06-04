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

#include "instruction_map.h"

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/instruction.h>
#include <sup/sequencer/instruction_tree.h>

namespace sup
{
namespace auto_server
{

InstructionMap::InstructionMap(const sequencer::Instruction* root)
  : m_instruction_map{}
{
  InitializeMap(root);
}

InstructionMap::~InstructionMap() = default;

sup::dto::uint32 InstructionMap::FindInstructionIndex(const sequencer::Instruction* instr) const
{
  auto iter = m_instruction_map.find(instr);
  if (iter == m_instruction_map.end())
  {
    std::string message = "InstructionMap::FindInstructionIndex(): unknown instruction";
    throw InvalidOperationException(message);
  }
  return iter->second;
}

std::map<const sequencer::Instruction*, sup::dto::uint32>
InstructionMap::GetInstructionMapping() const
{
  return m_instruction_map;
}

sup::dto::uint32 InstructionMap::GetNumberOfInstructions() const
{
  return m_instruction_map.size();
}

void InstructionMap::InitializeMap(const sequencer::Instruction* root)
{
  if (root == nullptr)
  {
    return;
  }
  auto instr_list = sequencer::FlattenBFS(sequencer::CreateFullInstructionTree(root));
  for (sup::dto::uint32 idx = 0; idx < instr_list.size(); ++idx)
  {
    m_instruction_map[instr_list[idx]] = idx;
  }
}

}  // namespace auto_server

}  // namespace sup
