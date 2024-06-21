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

#include "anyvalue_utils.h"
#include "instruction_map.h"

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/instruction.h>

#include <deque>

namespace
{
using namespace sup::auto_server;

struct InstructionInfoStackNode
{
  const sup::sequencer::Instruction* instruction;
  InstructionInfo* instr_info;
};

struct InstructionNode
{
  const sup::sequencer::Instruction* instruction;
  sup::dto::AnyValue& anyvalue;
  size_t idx;
};

sup::dto::AnyValue& AddChildAnyValue(sup::dto::AnyValue& parent, const sup::dto::AnyValue& child,
                                     std::size_t idx);
bool ValidateInstructionInfo(const sup::dto::AnyValue& instr_info);

}  // unnamed namespace

namespace sup
{
namespace auto_server
{
namespace utils
{
std::unique_ptr<InstructionInfo> CreateInstructionInfoTree(const sequencer::Instruction& root)
{
  std::deque<InstructionInfoStackNode> stack;
  auto root_address = std::addressof(root);
  InstructionMap instr_map{root_address};
  auto root_info = CreateInstructionInfoNode(root, instr_map.FindInstructionIndex(root_address));
  InstructionInfoStackNode root_node{ root_address, root_info.get() };
  stack.push_back(root_node);
  while (!stack.empty())
  {
    auto node = stack.back();
    stack.pop_back();
    auto children = node.instruction->ChildInstructions();
    for (auto child : children)
    {
      auto child_info = CreateInstructionInfoNode(*child, instr_map.FindInstructionIndex(child));
      auto child_info_p = node.instr_info->AppendChild(std::move(child_info));
      InstructionInfoStackNode child_node{child, child_info_p};
      stack.push_back(child_node);
    }
  }
  return root_info;
}

std::unique_ptr<InstructionInfo> CreateInstructionInfoNode(const sequencer::Instruction& instr,
                                                           sup::dto::uint32 index)
{
  auto instr_type = instr.GetType();
  std::vector<StringAttribute> attributes = instr.GetStringAttributes();
  return std::unique_ptr<InstructionInfo>{new InstructionInfo{instr_type, index, attributes}};
}

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

std::unique_ptr<InstructionInfo> ToInstructionInfo(const sup::dto::AnyValue& instr_info_anyvalue)
{
  if (!ValidateInstructionInfo(instr_info_anyvalue))
  {
    const std::string error = "ToInstructionInfo(): wrong format of instruction info AnyValue";
    throw InvalidOperationException(error);
  }
  auto instr_type = instr_info_anyvalue[kInstructionInfoNodeTypeField].As<std::string>();
  auto instr_idx = instr_info_anyvalue[kIndexField].As<sup::dto::uint32>();
  auto& attr_av = instr_info_anyvalue[kAttributesField];
  std::vector<StringAttribute> attributes;
  for (const auto& attr_name : attr_av.MemberNames())
  {
    attributes.emplace_back(attr_name, attr_av[attr_name].As<std::string>());
  }
  return std::unique_ptr<InstructionInfo>{new InstructionInfo(instr_type, instr_idx, attributes)};
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

bool ValidateInstructionInfo(const sup::dto::AnyValue& instr_info)
{
  if (!utils::ValidateMemberType(instr_info, kInstructionInfoNodeTypeField, sup::dto::StringType))
  {
    return false;
  }
  if (!utils::ValidateMemberType(instr_info, kIndexField, sup::dto::UnsignedInteger32Type))
  {
    return false;
  }
  if (!instr_info.HasField(kAttributesField) ||
      !sup::dto::IsStructValue(instr_info[kAttributesField]))
  {
    return false;
  }
  const auto& instr_attrs = instr_info[kAttributesField];
  auto mem_names = instr_attrs.MemberNames();
  for (const auto& mem_name : mem_names)
  {
    if (instr_attrs[mem_name].GetType() != sup::dto::StringType)
    {
      return false;
    }
  }
  return true;

}

}  // unnamed namespace
