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

#include "instruction_tree_cache.h"

#include <deque>

namespace sup
{
namespace auto_server
{

InstructionTreeCache::InstructionTreeCache(const sequencer::Instruction* root_instruction)
  : m_instruction_paths{}
  , m_proc_anyvalue{}
{}

InstructionTreeCache::~InstructionTreeCache() = default;

std::string InstructionTreeCache::GetInstructionPath(const sequencer::Instruction* instruction) const
{
  auto iter = m_instruction_paths.find(instruction);
  if (iter == m_instruction_paths.end())
  {
    return {};
  }
  return iter->second;
}

dto::AnyValue InstructionTreeCache::GetInitialProcedureAnyValue() const
{
  return m_proc_anyvalue;
}

}  // namespace auto_server

}  // namespace sup
