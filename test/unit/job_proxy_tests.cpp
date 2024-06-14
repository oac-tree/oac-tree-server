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

#include <sup/auto-server/job_info.h>
#include <sup/auto-server/job_proxy.h>
#include <sup/auto-server/job_value_mapper.h>
#include <sup/auto-server/job.h>

#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class JobProxyTest : public ::testing::Test
{
protected:
  JobProxyTest() = default;
  virtual ~JobProxyTest() = default;
};

TEST_F(JobProxyTest, Constructor)
{
  const std::string prefix = "JobProxyTest:Constructor:";
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);

  // Build mapper
  JobValueMapper mapper{prefix, *proc};
  auto root = proc->RootInstruction();
  mapper.InitializeInstructionTree(root);

  // Create Job and its JobInfo
  Job job{prefix, std::move(proc)};
  auto job_info = job.GetInfo();
  auto job_info_av = ToAnyValue(job_info);

  // Create and validate JobProxy
  JobProxy job_proxy{job_info_av};
  EXPECT_EQ(job_proxy.GetPrefix(), prefix);
  EXPECT_EQ(job_proxy.GetProcedureName(), "Common header");
  EXPECT_EQ(job_proxy.GetNumberOfVariables(), 3);
  EXPECT_EQ(job_proxy.GetNumberOfInstructions(), 3);
  auto& ws_proxy = job_proxy.GetWorkspaceInfo();
  EXPECT_EQ(ws_proxy.size(), 3);
  for (std::size_t idx = 0; idx < ws_proxy.size(); ++idx)
  {
    EXPECT_EQ(ws_proxy[idx].GetIndex(), idx);
  }
  auto instr_map = job_proxy.GetInstructionIndexMap();
  EXPECT_EQ(instr_map.size(), 3);
  for (std::size_t idx = 0; idx < instr_map.size(); ++idx)
  {
    EXPECT_EQ(instr_map[idx]->GetIndex(), idx);
  }
}
