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

#include <sup/auto-server/server_ui.h>

#include <sup/sequencer/application_utils.h>
#include <sup/sequencer/instruction.h>
#include <sup/sequencer/job_controller.h>
#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;
using namespace sup::sequencer;

const std::string kShortSequenceBody{
R"RAW(
  <Sequence>
    <Wait timeout="0.1"/>
    <Wait timeout="0.1"/>
  </Sequence>
  <Workspace/>
)RAW"};

class ServerUserInterfaceTest : public ::testing::Test
{
protected:
  ServerUserInterfaceTest() = default;
  virtual ~ServerUserInterfaceTest() = default;

  UnitTestHelper::TestJobPVServer m_pv_server;
  utils::SimpleJobStateMonitor m_monitor;
};

TEST_F(ServerUserInterfaceTest, InstructionStatusUpdates)
{
  // Construct procedure
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kShortSequenceBody);
  auto proc = ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Construct ui
  auto root_instr = proc->RootInstruction();
  ASSERT_NE(root_instr, nullptr);
  ServerUserInterface ui{m_pv_server};

  // Construct JobController and run procedure
  JobController controller{*proc, ui, m_monitor};
  EXPECT_TRUE(m_monitor.WaitForState(JobState::kInitial, 1.0));
  controller.Start();
  EXPECT_TRUE(m_monitor.WaitForState(JobState::kSucceeded, 2.0));
  EXPECT_EQ(m_pv_server.GetInstructionUpdateCount(ExecutionStatus::NOT_FINISHED), 3u);
  EXPECT_EQ(m_pv_server.GetInstructionUpdateCount(ExecutionStatus::SUCCESS), 3u);
}
