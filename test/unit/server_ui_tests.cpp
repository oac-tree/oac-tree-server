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

#include <sup/auto-server/server_job_interface.h>

#include <sup/sequencer/application_utils.h>
#include <sup/sequencer/instruction.h>
#include <sup/sequencer/job_controller.h>
#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;
using namespace sup::sequencer;

class ServerJobInterfaceTest : public ::testing::Test
{
protected:
  ServerJobInterfaceTest() = default;
  virtual ~ServerJobInterfaceTest() = default;

  UnitTestHelper::TestJobPVServer m_pv_server;
};

TEST_F(ServerJobInterfaceTest, InstructionStatusUpdates)
{
  // Construct procedure
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kShortSequenceBody);
  auto proc = ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Construct ui
  auto root_instr = proc->RootInstruction();
  ASSERT_NE(root_instr, nullptr);
  ServerJobInterface job_ui{m_pv_server};

  // Construct JobController and run procedure
  JobController controller{*proc, job_ui};
  EXPECT_TRUE(m_pv_server.WaitForState(JobState::kInitial, 1.0));
  controller.Start();
  EXPECT_TRUE(m_pv_server.WaitForState(JobState::kSucceeded, 2.0));
  EXPECT_EQ(m_pv_server.GetInstructionUpdateCount(ExecutionStatus::NOT_FINISHED), 3u);
  EXPECT_EQ(m_pv_server.GetInstructionUpdateCount(ExecutionStatus::SUCCESS), 3u);
}

TEST_F(ServerJobInterfaceTest, JobStateUpdates)
{
  // Construct procedure
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kShortSequenceBody);
  auto proc = ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Construct ui and monitor
  auto root_instr = proc->RootInstruction();
  ASSERT_NE(root_instr, nullptr);
  ServerJobInterface job_ui{m_pv_server};

  // Construct JobController and run procedure
  JobController controller{*proc, job_ui};
  EXPECT_EQ(m_pv_server.GetJobState(), JobState::kInitial);
  EXPECT_EQ(m_pv_server.GetBreakpointUpdateCount(), 0);
  controller.SetBreakpoint(root_instr);
  EXPECT_EQ(m_pv_server.GetBreakpointUpdateCount(), 1u);
  controller.Start();
  EXPECT_TRUE(m_pv_server.WaitForState(JobState::kPaused, 1.0));
  EXPECT_EQ(m_pv_server.GetJobState(), JobState::kPaused);
  controller.RemoveBreakpoint(root_instr);
  EXPECT_EQ(m_pv_server.GetBreakpointUpdateCount(), 2u);
  controller.Start();
  EXPECT_TRUE(m_pv_server.WaitForState(JobState::kSucceeded, 1.0));
  EXPECT_EQ(m_pv_server.GetJobState(), JobState::kSucceeded);
}

TEST_F(ServerJobInterfaceTest, VariableUpdates)
{
  // Construct procedure
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Construct ui
  auto root_instr = proc->RootInstruction();
  ASSERT_NE(root_instr, nullptr);
  ServerJobInterface job_ui{m_pv_server};

  // Construct JobController and run procedure
  JobController controller{*proc, job_ui};
  EXPECT_TRUE(m_pv_server.WaitForState(JobState::kInitial, 1.0));
  controller.Start();
  EXPECT_TRUE(m_pv_server.WaitForState(JobState::kSucceeded, 2.0));
  EXPECT_EQ(m_pv_server.GetVariableUpdateCount("var1"), 1u);
  EXPECT_EQ(m_pv_server.GetVariableUpdateCount("var2"), 1u);
}
