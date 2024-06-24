/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVER
 *
 * Description   : Unit test code
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

#include "unit_test_helper.h"

#include <sup/auto-server/base/instruction_tree_utils.h>
#include <sup/auto-server/base/instruction_map.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <set>

using namespace sup::auto_server;

class InstructionTreeUtilsTest : public ::testing::Test
{
protected:
  InstructionTreeUtilsTest() = default;
  virtual ~InstructionTreeUtilsTest() = default;
};

bool CheckInstructionNodeAnyValue(const sup::dto::AnyValue& node, const std::string& node_type,
                                  sup::dto::uint32 index,
                                  const sup::sequencer::StringAttributeList& attributes);

TEST_F(InstructionTreeUtilsTest, InstructionInfoFromInstructionTree)
{
  // Construct procedure and extract Workspace
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());
  const auto* root = proc->RootInstruction();

  // Create InstructionInfo tree
  InstructionMap instr_map{root};
  auto instr_info = utils::CreateInstructionInfoTree(*root, instr_map);

  // Check root of the InstructionInfo tree
  EXPECT_EQ(instr_info->GetType(), "Sequence");
  std::set<sup::dto::uint32> indices;
  indices.insert(instr_info->GetIndex());
  auto attributes = instr_info->GetAttributes();
  EXPECT_EQ(attributes.size(), 0);

  auto children = instr_info->Children();
  ASSERT_EQ(children.size(), 2);
  {
    // check first child
    auto child = children[0];
    EXPECT_EQ(child->GetType(), "Copy");
    indices.insert(child->GetIndex());
    auto child_attrs = child->GetAttributes();
    EXPECT_EQ(child_attrs.size(), 2);
    EXPECT_EQ(child->Children().size(), 0);
  }
  {
    // check second child
    auto child = children[1];
    EXPECT_EQ(child->GetType(), "Copy");
    indices.insert(child->GetIndex());
    auto child_attrs = child->GetAttributes();
    EXPECT_EQ(child_attrs.size(), 2);
    EXPECT_EQ(child->Children().size(), 0);
  }
  std::set<sup::dto::uint32> expected_indices{0,1,2};
  EXPECT_EQ(indices, expected_indices);
}

TEST_F(InstructionTreeUtilsTest, InstructionInfoToFromAnyValue)
{
  // Construct procedure and extract Workspace
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());
  const auto* root = proc->RootInstruction();

  // Create InstructionInfo tree
  InstructionMap instr_map{root};
  auto instr_info = utils::CreateInstructionInfoTree(*root, instr_map);

  // Create AnyValue, translate back and check they are equal
  auto instr_av = utils::ToAnyValueTree(*instr_info);
  auto instr_info_read_back = utils::ToInstructionInfoTree(instr_av);
  EXPECT_EQ(*instr_info_read_back, *instr_info);
}

TEST_F(InstructionTreeUtilsTest, BuildInstructionTreeInfo)
{
  // Construct procedure
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Construct value mapper
  const std::string prefix = "Test_BuildInstructionTreeInfo:";
  JobValueMapper mapper{prefix, *proc};
  mapper.InitializeInstructionTree(proc->RootInstruction());

  // Create instruction tree info AnyValue
  auto instr_tree = utils::BuildInstructionTreeInfo(proc->RootInstruction(), mapper);

  // Check this AnyValue
  EXPECT_TRUE(CheckInstructionNodeAnyValue(instr_tree, "Sequence", 0, {}));
  ASSERT_TRUE(instr_tree.HasField(kChildInstructionsField));
  auto& children = instr_tree[kChildInstructionsField];
  ASSERT_EQ(children.NumberOfMembers(), 2);
  ASSERT_TRUE(children.HasField(utils::CreateIndexedInstrChildName(0)));
  ASSERT_TRUE(children.HasField(utils::CreateIndexedInstrChildName(1)));
  auto& child_0 = children[utils::CreateIndexedInstrChildName(0)];
  sup::sequencer::StringAttributeList child_0_attrs{
    { "inputVar", "one" },
    { "outputVar", "var1" }
  };
  EXPECT_TRUE(CheckInstructionNodeAnyValue(child_0, "Copy", 1, child_0_attrs));
  auto& child_1 = children[utils::CreateIndexedInstrChildName(1)];
  sup::sequencer::StringAttributeList child_1_attrs{
    { "inputVar", "one" },
    { "outputVar", "var2" }
  };
  EXPECT_TRUE(CheckInstructionNodeAnyValue(child_1, "Copy", 2, child_1_attrs));
}

bool CheckInstructionNodeAnyValue(const sup::dto::AnyValue& node, const std::string& node_type,
                                  sup::dto::uint32 index,
                                  const sup::sequencer::StringAttributeList& attributes)
{
  if (!node.HasField(kInstructionInfoNodeTypeField) ||
      !node.HasField(kIndexField) || !node.HasField(kAttributesField))
  {
    return false;
  }
  if (node[kInstructionInfoNodeTypeField] != node_type)
  {
    return false;
  }
  if (node[kIndexField] != index)
  {
    return false;
  }
  auto& attr_av = node[kAttributesField];
  for (const auto& attr : attributes)
  {
    if (!attr_av.HasField(attr.first))
    {
      return false;
    }
    if (attr_av[attr.first] != attr.second)
    {
      return false;
    }
  }
  return attr_av.NumberOfMembers() == attributes.size();
}