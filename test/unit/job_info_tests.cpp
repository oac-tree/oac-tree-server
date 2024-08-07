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

#include <sup/auto-server/instruction_map.h>
#include <sup/auto-server/instruction_tree_utils.h>
#include <sup/auto-server/job_value_mapper.h>
#include <sup/auto-server/job_utils.h>
#include <sup/auto-server/server_job.h>
#include <sup/auto-server/sup_auto_protocol.h>
#include <sup/auto-server/variable_utils.h>

#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class JobInfoTest : public ::testing::Test
{
protected:
  JobInfoTest() = default;
  virtual ~JobInfoTest() = default;
};

TEST_F(JobInfoTest, FromProcedure)
{
  const std::string prefix = "JobInfoTest:FromProcedure:";
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);

  // Build job info representation
  auto root = proc->RootInstruction();
  InstructionMap instr_map{root};
  auto job_info = utils::CreateJobInfo(prefix, *proc, instr_map);

  EXPECT_EQ(job_info.GetPrefix(), prefix);
  EXPECT_EQ(job_info.GetProcedureName(), "Common header");
  EXPECT_EQ(job_info.GetNumberOfVariables(), 3);
  // EXPECT_EQ(job_info.GetNumberOfInstructions(), 3);

  // Validate WorkspaceInfo:
  auto ws_info = job_info.GetWorkspaceInfo();
  EXPECT_EQ(ws_info.GetNumberOfVariables(), 3);
  const auto& vars = ws_info.GetVariableInfos();
  ASSERT_EQ(vars.size(), 3);
  {
    // Test first variable info
    EXPECT_EQ(vars[0].first, "one");
    const auto& var_info = vars[0].second;
    EXPECT_EQ(var_info.GetType(), "Local");
    EXPECT_EQ(var_info.GetIndex(), 0u);
    auto var_attributes = var_info.GetAttributes();
    EXPECT_EQ(var_attributes.size(), 3);
  }
  {
    // Test second variable info
    EXPECT_EQ(vars[1].first, "var1");
    const auto& var_info = vars[1].second;
    EXPECT_EQ(var_info.GetType(), "Local");
    EXPECT_EQ(var_info.GetIndex(), 1u);
    auto var_attributes = var_info.GetAttributes();
    EXPECT_EQ(var_attributes.size(), 3);
  }
  {
    // Test third variable info
    EXPECT_EQ(vars[2].first, "var2");
    const auto& var_info = vars[2].second;
    EXPECT_EQ(var_info.GetType(), "Local");
    EXPECT_EQ(var_info.GetIndex(), 2u);
    auto var_attributes = var_info.GetAttributes();
    EXPECT_EQ(var_attributes.size(), 3);
  }
  // Validate InstructionInfo tree
  auto instr_info = job_info.GetRootInstructionInfo();
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

TEST_F(JobInfoTest, JobInfoToFromAnyValue)
{
  // Construct procedure
  const std::string prefix = "JobInfoTest:JobInfoToFromAnyValue:";
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Build job info representation
  auto root = proc->RootInstruction();
  InstructionMap instr_map{root};
  auto job_info = utils::CreateJobInfo(prefix, *proc, instr_map);

  // Create AnyValue, translate back and check they are equal
  auto job_av = utils::ToAnyValue(job_info);
  auto job_info_read_back = utils::ToJobInfo(job_av);
  EXPECT_EQ(job_info_read_back, job_info);
}
