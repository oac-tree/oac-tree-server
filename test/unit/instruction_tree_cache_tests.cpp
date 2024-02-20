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

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/instruction_tree_cache.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/sequencer/instruction.h>
#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

#include <iostream>

using namespace sup::auto_server;

const std::string kProcedureBody{
R"RAW(
<Sequence>
    <Sequence>
        <Wait/>
        <Wait/>
        <Wait/>
    </Sequence>
    <Sequence>
        <Wait/>
        <Wait/>
    </Sequence>
    <Inverter>
        <Sequence>
            <Wait/>
            <Wait/>
        </Sequence>
    </Inverter>
    <Sequence>
        <Wait/>
        <Wait/>
        <Wait/>
    </Sequence>
</Sequence>
<Workspace/>
)RAW"};

class InstructionTreeCacheTest : public ::testing::Test
{
protected:
  InstructionTreeCacheTest() = default;
  virtual ~InstructionTreeCacheTest() = default;

};

namespace
{
// This helper function dumps the InstructionCache to std::cout for debugging purposes.
void DumpInstructionTreeCache(const InstructionTreeCache& tree_cache);
}

TEST_F(InstructionTreeCacheTest, Construction)
{
  const auto procedure_string = sup::UnitTestHelper::CreateProcedureString(kProcedureBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());
  auto root_instr = proc->RootInstruction();
  ASSERT_NE(root_instr, nullptr);
  InstructionTreeCache tree_cache{root_instr};
  auto instruction_map = tree_cache.GetInstructionPaths();
  auto tree_anyvalue = tree_cache.GetInitialInstructionTreeAnyValue();
  EXPECT_FALSE(sup::dto::IsEmptyValue(tree_anyvalue));
  EXPECT_EQ(instruction_map.size(), 16);
  std::set<std::string> instr_paths;
  for (const auto& entry : instruction_map)
  {
    std::string path;
    EXPECT_NO_THROW(path = tree_cache.FindInstructionPath(entry.first));
    instr_paths.insert(path);
    if (entry.second.empty())
    {
      continue;
    }
    EXPECT_TRUE(tree_anyvalue.HasField(entry.second));
  }
  EXPECT_EQ(instruction_map.size(), instr_paths.size());
  // DumpInstructionTreeCache(tree_cache);
}

TEST_F(InstructionTreeCacheTest, Exceptions)
{
  // InstructionTreeCache constructor throws when given a nullptr
  EXPECT_THROW(InstructionTreeCache{nullptr}, InvalidOperationException);

  // InstructionTreeCache::FindInstructionPath throws for unknown instruction
  const auto procedure_string = sup::UnitTestHelper::CreateProcedureString(kProcedureBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());
  auto root_instr = proc->RootInstruction();
  ASSERT_NE(root_instr, nullptr);
  InstructionTreeCache tree_cache{root_instr};
  EXPECT_THROW(tree_cache.FindInstructionPath(nullptr), InvalidOperationException);
}

TEST_F(InstructionTreeCacheTest, PathFormat)
{
  // Check format of paths.
  const auto procedure_string = sup::UnitTestHelper::CreateProcedureString(kProcedureBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());
  auto root_instr = proc->RootInstruction();
  ASSERT_NE(root_instr, nullptr);
  InstructionTreeCache tree_cache{root_instr};

  // Check that path for root instruction is just the empty string:
  EXPECT_EQ(tree_cache.FindInstructionPath(root_instr), std::string{});

  // Check some child instruction paths:
  auto children_1 = root_instr->ChildInstructions();
  ASSERT_EQ(children_1.size(), 4);
  auto first_seq = children_1[0];
  EXPECT_EQ(tree_cache.FindInstructionPath(first_seq), std::string{"children.Sequence0"});
  auto children_2 = first_seq->ChildInstructions();
  ASSERT_EQ(children_2.size(), 3);
  auto third_wait = children_2[2];
  EXPECT_EQ(tree_cache.FindInstructionPath(third_wait),
            std::string{"children.Sequence0.children.Wait2"});
}

namespace
{
void DumpInstructionTreeCache(const InstructionTreeCache& tree_cache)
{
  std::cout << "Map of instruction paths:" << std::endl;
  std::cout << "=========================" << std::endl;
  auto instr_map = tree_cache.GetInstructionPaths();
  for (const auto& entry : instr_map)
  {
    std::cout << entry.second << std::endl;
  }
  std::cout << std::endl;
  std::cout << "Dump of AnyValue:" << std::endl;
  std::cout << "=================" << std::endl;
  std::cout << sup::dto::PrintAnyValue(tree_cache.GetInitialInstructionTreeAnyValue()) << std::endl;
}
}
