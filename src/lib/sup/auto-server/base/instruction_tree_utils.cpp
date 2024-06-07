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

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/instruction.h>

#include <deque>

namespace
{
using namespace sup::auto_server;

struct InstructionNode
{
  const sup::sequencer::Instruction* instruction;
  sup::dto::AnyValue& anyvalue;
  size_t idx;
};

sup::dto::AnyValue& AddChildAnyValue(sup::dto::AnyValue& parent, const sup::dto::AnyValue& child,
                                     std::size_t idx);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{
namespace utils
{

sup::dto::AnyValue BuildInstructionTreeInfo(const sequencer::Instruction* root,
                                            const JobValueMapper& mapper)
{
  std::deque<InstructionNode> stack;
  auto root_av = BuildInstructionInfoNode(root, mapper.GetInstructionIndex(root));
  InstructionNode root_node{ root, root_av, 0 };
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
    auto child_index = mapper.GetInstructionIndex(child);
    auto child_av = BuildInstructionInfoNode(child, child_index);
    auto& child_av_ref = AddChildAnyValue(node.anyvalue, child_av, node.idx);
    stack.push_back(InstructionNode{ child, child_av_ref, 0 });
    ++node.idx;
  }
  return root_av;
}

sup::dto::AnyValue BuildInstructionInfoNode(const sequencer::Instruction* instr,
                                            sup::dto::uint32 index)
{
  if (instr == nullptr)
  {
    const std::string error = "BuildInstructionInfoNode(): called with a nullptr";
    throw InvalidOperationException(error);
  }
  auto result = kInstructionInfoNodeAnyValue;
  result[kInstructionInfoNodeTypeField] = instr->GetType();
  result[kIndexField] = index;
  for (const auto& attr : instr->GetStringAttributes())
  {
    result[kAttributesField].AddMember(attr.first, attr.second);
  }
  return result;
}

std::string CreateIndexedMemberName(std::size_t idx)
{
  return kChildMemberFieldPrefix + std::to_string(idx);
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

namespace
{
using namespace sup::auto_server;

sup::dto::AnyValue& AddChildAnyValue(sup::dto::AnyValue& parent, const sup::dto::AnyValue& child,
                                     std::size_t idx)
{
  if (!parent.HasField(kChildInstructionsField))
  {
    parent.AddMember(kChildInstructionsField, sup::dto::EmptyStruct());
  }
  auto member_name = utils::CreateIndexedMemberName(idx);
  parent[kChildInstructionsField].AddMember(member_name, child);
  return parent[kChildInstructionsField][member_name];
}
}  // unnamed namespace
