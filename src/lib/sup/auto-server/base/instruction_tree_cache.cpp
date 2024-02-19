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

#include "instruction_tree_utils.h"

#include <sup/sequencer/instruction.h>

#include <deque>

namespace
{
struct InstructionNode
{
  const sup::sequencer::Instruction* instruction;
  std::string path;
  size_t idx;
};

std::string PushInstructionNode(std::deque<InstructionNode>& stack,
                                std::set<std::string>& path_names,
                                const sup::sequencer::Instruction* instruction,
                                const std::string& parent_path);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{

InstructionTreeCache::InstructionTreeCache(const sequencer::Instruction* root_instruction)
  : m_instruction_paths{}
  , m_proc_anyvalue{}
{
  InitializeCache(root_instruction);
}

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

std::map<const sequencer::Instruction*, std::string>
InstructionTreeCache::GetInstructionPaths() const
{
  return m_instruction_paths;
}

dto::AnyValue InstructionTreeCache::GetInitialProcedureAnyValue() const
{
  return m_proc_anyvalue;
}

void InstructionTreeCache::InitializeCache(const sequencer::Instruction* root_instruction)
{
  if (root_instruction == nullptr)
  {
    std::string message = "InstructionTreeCache::InitializeCache(): called with nullptr";
    // TODO: throw appropriate exception
    throw 0;
  }
  std::set<std::string> path_names;
  std::deque<InstructionNode> stack;
  m_instruction_paths[root_instruction] =
    PushInstructionNode(stack, path_names, root_instruction, "");
  while (!stack.empty())
  {
    auto& node = stack.back();
    auto children = node.instruction->ChildInstructions();
    if (node.idx >= children.size())
    {
      stack.pop_back();
      continue;
    }
    auto child = children[node.idx];
    ++node.idx;
    m_instruction_paths[child] = PushInstructionNode(stack, path_names, child, node.path);
  }
}

}  // namespace auto_server

}  // namespace sup

namespace
{
using namespace sup::auto_server;

std::string PushInstructionNode(std::deque<InstructionNode>& stack,
                                std::set<std::string>& path_names,
                                const sup::sequencer::Instruction* instruction,
                                const std::string& parent_path)
{
  auto instr_path = utils::CreateUniquePath(instruction, parent_path, path_names);
  InstructionNode node{ instruction, instr_path, 0 };
  stack.push_back(node);
  return instr_path;
}
}  // unnamed namespace

