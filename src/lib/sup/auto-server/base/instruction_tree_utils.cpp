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
#include <set>

namespace
{
using namespace sup::sequencer;
std::string CreateUniquePath(const Instruction* instruction,
                             const std::string& prefix,
                             std::set<std::string>& used_names);
}  // unnamed namespace

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
  auto root_path = CreateUniquePath(root, "", path_names);
  InstructionNode root_node{ root, root_path, 0 };
  result[root] = root_path;
  std::deque<InstructionNode> stack;
  stack.push_back(root_node);
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
    auto child_path = CreateUniquePath(child, node.path, path_names);
    InstructionNode child_node{ child, child_path, 0 };
    result[child] = child_path;
    stack.push_back(child_node);
  }
  return result;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

namespace
{
std::string CreateUniquePath(const Instruction* instruction,
                             const std::string& prefix,
                             std::set<std::string>& used_names)
{
  auto instr_type = instruction->GetType();
  std::string field_base = prefix + "." + instr_type;
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
}  // unnamed namespace
