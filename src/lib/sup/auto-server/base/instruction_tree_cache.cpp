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
using namespace sup::auto_server;

const sup::dto::AnyValue kInstructionAnyValue = {{
  { utils::kExecStatusField, static_cast<sup::dto::uint16>(sup::sequencer::ExecutionStatus::NOT_STARTED)},
  { utils::kBreakpointField, false },
  { utils::kChildrenField, sup::dto::EmptyStruct() }
}, utils::kInstructionNodeType };

struct InstructionNode
{
  const sup::sequencer::Instruction* instruction;
  sup::dto::AnyValue& anyvalue;
  std::string path;
  size_t idx;
};

std::string PushRootNode(std::deque<InstructionNode>& stack,
                         sup::dto::AnyValue& root_anyvalue,
                         const sup::sequencer::Instruction* root_instruction);

std::string PushInstructionNode(std::deque<InstructionNode>& stack,
                                std::set<std::string>& path_names,
                                const sup::sequencer::Instruction* instruction);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{

InstructionTreeCache::InstructionTreeCache(const sequencer::Instruction* root_instruction)
  : m_instruction_paths{}
  , m_proc_anyvalue{kInstructionAnyValue}
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
  m_instruction_paths[root_instruction] = PushRootNode(stack, m_proc_anyvalue, root_instruction);
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
    m_instruction_paths[child] = PushInstructionNode(stack, path_names, child);
  }
}

}  // namespace auto_server

}  // namespace sup

namespace
{
using namespace sup::auto_server;

std::string PushRootNode(std::deque<InstructionNode>& stack,
                         sup::dto::AnyValue& root_anyvalue,
                         const sup::sequencer::Instruction* root_instruction)
{
  root_anyvalue = kInstructionAnyValue;
  InstructionNode node{ root_instruction, root_anyvalue, "", 0 };
  stack.push_back(node);
  return "";
}

std::string PushInstructionNode(std::deque<InstructionNode>& stack,
                                std::set<std::string>& path_names,
                                const sup::sequencer::Instruction* instruction)
{
  if (stack.empty())
  {
    throw 0;
  }
  auto& parent_node = stack.back();
  std::string parent_path = parent_node.path;
  auto instr_path = utils::CreateUniqueField(instruction, parent_path, path_names);
  auto full_instr_path = utils::CreateFullInstructionPath(parent_path, instr_path);
  parent_node.anyvalue[utils::kChildrenField].AddMember(instr_path, kInstructionAnyValue);
  InstructionNode node{ instruction, parent_node.anyvalue[utils::kChildrenField][instr_path],
                        full_instr_path, 0 };
  stack.push_back(node);
  return full_instr_path;
}
}  // unnamed namespace
