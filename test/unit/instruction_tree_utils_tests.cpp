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
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

#include <iostream>

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
  ASSERT_TRUE(children.HasField(utils::CreateIndexedMemberName(0)));
  ASSERT_TRUE(children.HasField(utils::CreateIndexedMemberName(1)));
  auto& child_0 = children[utils::CreateIndexedMemberName(0)];
  sup::sequencer::StringAttributeList child_0_attrs{
    { "inputVar", "one" },
    { "outputVar", "var1" }
  };
  EXPECT_TRUE(CheckInstructionNodeAnyValue(child_0, "Copy", 1, child_0_attrs));
  auto& child_1 = children[utils::CreateIndexedMemberName(1)];
  sup::sequencer::StringAttributeList child_1_attrs{
    { "inputVar", "one" },
    { "outputVar", "var2" }
  };
  EXPECT_TRUE(CheckInstructionNodeAnyValue(child_1, "Copy", 2, child_1_attrs));

  std::cout << sup::dto::PrintAnyValue(instr_tree) << std::endl;
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