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

#include <sup/auto-server/automation_server.h>
#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

const std::string kTestPrefix = "AutomationServerTests";

class AutomationServerTests : public ::testing::Test
{
protected:
  AutomationServerTests() = default;
  virtual ~AutomationServerTests() = default;

  UnitTestHelper::TestServerInterface m_test_server_interface;
};

TEST_F(AutomationServerTests, ServerWithoutProcedures)
{
  using sup::sequencer::JobCommand;
  const std::string prefix = "AutomationServerTests:Empty";
  AutomationServer auto_server{prefix};
  EXPECT_EQ(auto_server.GetServerPrefix(), prefix);
  EXPECT_EQ(auto_server.GetNumberOfJobs(), 0);
  EXPECT_THROW(auto_server.GetJobInfo(0), InvalidOperationException);
  EXPECT_THROW(auto_server.SendJobCommand(0, JobCommand::kStart), InvalidOperationException);
}

TEST_F(AutomationServerTests, ServerWithOneProcedure)
{
  using sup::sequencer::JobCommand;
  const std::string prefix = "AutomationServerTests:Single";
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kLongWaitProcedureBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  AutomationServer auto_server{prefix};
  auto_server.AddJob(std::move(proc), m_test_server_interface);
  EXPECT_EQ(auto_server.GetServerPrefix(), prefix);
  EXPECT_EQ(auto_server.GetNumberOfJobs(), 1);
  const auto& job_info = auto_server.GetJobInfo(0);
  EXPECT_EQ(job_info.GetPrefix(), CreateJobPrefix(prefix, 0));
  EXPECT_EQ(job_info.GetProcedureName(), "Common header");
  EXPECT_EQ(job_info.GetNumberOfVariables(), 0);
  EXPECT_THROW(auto_server.GetJobInfo(1), InvalidOperationException);
  EXPECT_NO_THROW(auto_server.SendJobCommand(0, JobCommand::kStart));
  EXPECT_THROW(auto_server.SendJobCommand(1, JobCommand::kStart), InvalidOperationException);
}

TEST_F(AutomationServerTests, ServerWithTwoProcedures)
{
  using sup::sequencer::JobCommand;
  const std::string prefix = "AutomationServerTests:Two";
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kLongWaitProcedureBody);
  auto proc_1 = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc_1.get(), nullptr);
  auto proc_2 = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc_2.get(), nullptr);
  AutomationServer auto_server{prefix};
  auto_server.AddJob(std::move(proc_1), m_test_server_interface);
  auto_server.AddJob(std::move(proc_2), m_test_server_interface);
  EXPECT_EQ(auto_server.GetServerPrefix(), prefix);
  EXPECT_EQ(auto_server.GetNumberOfJobs(), 2);
  const auto& job_info_1 = auto_server.GetJobInfo(0);
  EXPECT_EQ(job_info_1.GetPrefix(), CreateJobPrefix(prefix, 0));
  EXPECT_EQ(job_info_1.GetProcedureName(), "Common header");
  EXPECT_EQ(job_info_1.GetNumberOfVariables(), 0);
  const auto& job_info_2 = auto_server.GetJobInfo(1);
  EXPECT_EQ(job_info_2.GetPrefix(), CreateJobPrefix(prefix, 1));
  EXPECT_EQ(job_info_2.GetProcedureName(), "Common header");
  EXPECT_EQ(job_info_2.GetNumberOfVariables(), 0);
  EXPECT_THROW(auto_server.GetJobInfo(2), InvalidOperationException);
  EXPECT_NO_THROW(auto_server.SendJobCommand(0, JobCommand::kStart));
  EXPECT_NO_THROW(auto_server.SendJobCommand(1, JobCommand::kStart));
  EXPECT_THROW(auto_server.SendJobCommand(2, JobCommand::kStart), InvalidOperationException);
}
