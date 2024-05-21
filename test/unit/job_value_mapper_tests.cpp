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
  const std::string prefix = "Test_GetSimpleMappings";
  JobValueMapper mapper{prefix, *proc};

  // Check value names
  EXPECT_EQ(mapper.GetInstructionTreeName(), prefix + kInstructionTreeId);
  EXPECT_EQ(mapper.GetJobStateName(), prefix + kJobStateId);

  // Check variable names
  auto var_names = mapper.GetVariableValueNames();
  EXPECT_EQ(var_names.size(), 3);

  auto var_name = mapper.GetVariableValueName("one");
  EXPECT_TRUE(StringListContains(var_names, var_name));
  var_name = mapper.GetVariableValueName("var1");
  EXPECT_TRUE(StringListContains(var_names, var_name));
  var_name = mapper.GetVariableValueName("var2");
  EXPECT_TRUE(StringListContains(var_names, var_name));
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
  EXPECT_EQ(mapper.GetInstructionTreeName(), prefix + kInstructionTreeId);
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
}

namespace
{
bool StringListContains(const std::vector<std::string>& list, const std::string& str)
{
  auto iter = std::find(list.begin(), list.end(), str);
  return iter != list.end();
}
}  // unnamed namespace

