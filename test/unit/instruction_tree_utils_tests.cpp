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

class InstructionTreeUtilsTest : public ::testing::Test
{
protected:
  InstructionTreeUtilsTest() = default;
  virtual ~InstructionTreeUtilsTest() = default;

};

namespace
{
void DumpInstructionMap(const std::map<const sup::sequencer::Instruction*, std::string>& instr_map);
}

TEST_F(InstructionTreeUtilsTest, CreateInstructionPaths)
{
  const auto procedure_string = sup::UnitTestHelper::CreateProcedureString(kProcedureBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());
  auto root_instr = proc->RootInstruction();
  ASSERT_NE(root_instr, nullptr);
  auto instruction_map = utils::CreateInstructionPaths(root_instr);
  EXPECT_EQ(instruction_map.size(), 16);
  DumpInstructionMap(instruction_map);
}

namespace
{
void DumpInstructionMap(const std::map<const sup::sequencer::Instruction*, std::string>& instr_map)
{
  for (const auto& entry : instr_map)
  {
    std::cout << entry.second << std::endl;
  }
}
}
