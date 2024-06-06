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

#include <sup/auto-server/job.h>
#include <sup/auto-server/job_value_mapper.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/auto-server/base/instruction_tree_utils.h>
#include <sup/auto-server/base/variable_utils.h>

#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class JobInfoTest : public ::testing::Test
{
protected:
  JobInfoTest() = default;
  virtual ~JobInfoTest() = default;
};

TEST_F(JobInfoTest, FromJob)
{
  const std::string prefix = "JobInfoTest:FromJob:";
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);

  // Build workspace representation before passing the procedure inside a job
  auto ws_av = utils::BuildWorkspaceInfo(proc->GetWorkspace());

  // Build instruction tree info representation
  JobValueMapper mapper{prefix, *proc};
  auto root = proc->RootInstruction();
  mapper.InitializeInstructionTree(root);
  auto instr_tree_info = utils::BuildInstructionTreeInfo(root, mapper);

  // Create Job and validate its JobInfo
  Job job{prefix, std::move(proc)};

  auto job_info = job.GetInfo();
  EXPECT_EQ(job_info.GetPrefix(), prefix);
  EXPECT_EQ(job_info.GetProcedureName(), "Common header");
  EXPECT_EQ(job_info.GetNumberOfVariables(), 3);
  EXPECT_EQ(job_info.GetNumberOfInstructions(), 3);
  EXPECT_EQ(job_info.GetVariableInfo(), ws_av);
  EXPECT_EQ(job_info.GetInstructionTreeInfo(), instr_tree_info);
}
