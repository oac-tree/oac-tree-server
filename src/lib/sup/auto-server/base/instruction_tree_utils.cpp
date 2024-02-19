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

#include "instruction_tree_utils.h"

#include <sup/sequencer/instruction.h>

#include <deque>

namespace sup
{
namespace auto_server
{
namespace utils
{

struct InstructionNode
{
  const sequencer::Instruction* instruction;
  std::string path;
  size_t idx;
};

std::string PushInstructionNode(std::deque<InstructionNode>& stack,
                                std::set<std::string>& path_names,
                                const sequencer::Instruction* instruction,
                                const std::string& parent_path)
{
  auto instr_path = CreateUniquePath(instruction, parent_path, path_names);
  InstructionNode node{ instruction, instr_path, 0 };
  stack.push_back(node);
  return instr_path;
}

std::map<const sequencer::Instruction*, std::string> CreateInstructionPaths(
  const sequencer::Instruction* root)
{
  if (root == nullptr)
  {
    std::string message = "CreateInstructionPaths(): called with nullptr";
    // TODO: throw appropriate exception
    throw 0;
  }
  std::map<const sequencer::Instruction*, std::string> result;
  std::set<std::string> path_names;
  std::deque<InstructionNode> stack;
  result[root] = PushInstructionNode(stack, path_names, root, "");
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
    result[child] = PushInstructionNode(stack, path_names, child, node.path);
  }
  return result;
}

std::string CreateUniquePath(const sequencer::Instruction* instruction,
                             const std::string& prefix,
                             std::set<std::string>& used_names)
{
  auto instr_type = instruction->GetType();
  auto full_prefix = prefix.empty() ? prefix
                                    : prefix + "." + kChildrenField + ".";
  std::string field_base = full_prefix + instr_type;
  size_t index = 0;
  std::string suggestion = field_base + std::to_string(index);
  auto iter = used_names.find(suggestion);
  while (iter != used_names.end())
  {
    index++;
    suggestion = field_base + std::to_string(index);
    iter = used_names.find(suggestion);
  }
  used_names.insert(suggestion);
  return suggestion;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
