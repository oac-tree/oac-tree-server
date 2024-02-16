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

InstructionNode CreateInstructionNode(const sequencer::Instruction* instruction,
                                      const std::string& prefix)
{
  std::string path = prefix + "."; // TODO: + instruction name??
  return { instruction, path, 0 };
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
  std::deque<InstructionNode> stack;
  stack.push_back(CreateInstructionNode(root, "root"));
  while (!stack.empty())
  {
    auto node = stack.back();
    stack.pop_back();
    result.emplace(node.instruction, node.path);
    for (auto instr : node.instruction->ChildInstructions())
    {
      stack.push_back(CreateInstructionNode(instr, node.path));
    }
  }
  return result;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
