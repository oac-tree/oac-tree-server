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
#include <sup/auto-server/job_value_mapper.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/sequence_parser.h>

#include <algorithm>

#include <gtest/gtest.h>

using namespace sup::auto_server;
using namespace sup::sequencer;

namespace
{
bool StringListContains(const std::vector<std::string>& list, const std::string& str);
}  // unnamed namespace

class JobValueMapperTest : public ::testing::Test
{
protected:
  JobValueMapperTest() = default;
  virtual ~JobValueMapperTest() = default;
};

TEST_F(JobValueMapperTest, GetSimpleMappings)
{
  // Construct procedure
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Construct value mapper
  const std::string prefix = "Test_GetSimpleMappings:";
  JobValueMapper mapper{prefix, *proc};

  // Check value names
  EXPECT_EQ(mapper.GetJobStateName(), prefix + kJobStateId);

  // Check variable names
  EXPECT_EQ(mapper.GetNumberOfVariables(), 3);
  auto var_names = mapper.GetVariableValueNames();
  EXPECT_EQ(var_names.size(), 3);
  EXPECT_EQ(mapper.GetVariableIndex("one"), 0);
  auto var_name = mapper.GetVariableValueName("one");
  EXPECT_TRUE(StringListContains(var_names, var_name));
  EXPECT_EQ(mapper.GetVariableIndex("var1"), 1);
  var_name = mapper.GetVariableValueName("var1");
  EXPECT_TRUE(StringListContains(var_names, var_name));
  EXPECT_EQ(mapper.GetVariableIndex("var2"), 2);
  var_name = mapper.GetVariableValueName("var2");
  EXPECT_TRUE(StringListContains(var_names, var_name));

  // Check absence of instruction values when the instruction tree was not passed yet
  EXPECT_EQ(mapper.GetNumberOfInstructions(), 0);
  EXPECT_THROW(mapper.GetInstructionIndex(nullptr), sup::auto_server::InvalidOperationException);
  EXPECT_THROW(mapper.GetInstructionValueName(nullptr), sup::auto_server::InvalidOperationException);
  auto instr_names = mapper.GetInstructionValueNames();
  EXPECT_EQ(instr_names.size(), 0);
}

TEST_F(JobValueMapperTest, GetInstructionMappings)
{
  // Construct procedure
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Construct value mapper
  const std::string prefix = "Test_GetInstructionMappings:";
  JobValueMapper mapper{prefix, *proc};

  // Check absence of instruction values when the instruction tree was not passed yet
  EXPECT_EQ(mapper.GetNumberOfInstructions(), 0);
  EXPECT_THROW(mapper.GetInstructionIndex(nullptr), sup::auto_server::InvalidOperationException);
  EXPECT_THROW(mapper.GetInstructionValueName(nullptr), sup::auto_server::InvalidOperationException);
  auto instr_names = mapper.GetInstructionValueNames();
  EXPECT_EQ(instr_names.size(), 0);

  // Initialize instruction tree and check again
  EXPECT_NO_THROW(mapper.InitializeInstructionTree(proc->RootInstruction()));
  EXPECT_EQ(mapper.GetNumberOfInstructions(), 3);
  EXPECT_EQ(mapper.GetInstructionIndex(proc->RootInstruction()), 0);
  EXPECT_EQ(mapper.GetInstructionValueName(proc->RootInstruction()), prefix + kInstructionId + "0");
  instr_names = mapper.GetInstructionValueNames();
  EXPECT_EQ(instr_names.size(), 3);
}

TEST_F(JobValueMapperTest, EmptyWorkspace)
{
  // Construct procedure
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kShortSequenceBody);
  auto proc = ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Construct value mapper
  const std::string prefix = "Test_EmptyWorkspace";
  JobValueMapper mapper{prefix, *proc};

  // Check value names
  EXPECT_EQ(mapper.GetJobStateName(), prefix + kJobStateId);

  // Check variable names
  auto var_names = mapper.GetVariableValueNames();
  EXPECT_EQ(var_names.size(), 0);
}

TEST_F(JobValueMapperTest, Exceptions)
{
  // Construct procedure
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kShortSequenceBody);
  auto proc = ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Construct value mapper
  const std::string prefix = "Test_Exceptions";
  JobValueMapper mapper{prefix, *proc};

  // Check value names
  EXPECT_THROW(mapper.GetVariableValueName("does_not_exist"),
               sup::auto_server::InvalidOperationException);

  // Check non-existent instructions
  EXPECT_NO_THROW(mapper.InitializeInstructionTree(proc->RootInstruction()));
  EXPECT_EQ(mapper.GetNumberOfInstructions(), 3);
  EXPECT_THROW(mapper.GetInstructionIndex(nullptr), sup::auto_server::InvalidOperationException);
  EXPECT_THROW(mapper.GetInstructionValueName(nullptr), sup::auto_server::InvalidOperationException);
}

namespace
{
bool StringListContains(const std::vector<std::string>& list, const std::string& str)
{
  auto iter = std::find(list.begin(), list.end(), str);
  return iter != list.end();
}
}  // unnamed namespace

