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

#include <sup/auto-server/client_anyvalue_manager.h>
#include <sup/auto-server/server_job_info_io.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Exactly;
using ::testing::InSequence;

using namespace sup::auto_server;

class JobInfoIOServerClientTest : public ::testing::Test
{
protected:
  JobInfoIOServerClientTest() = default;
  virtual ~JobInfoIOServerClientTest() = default;

  UnitTestHelper::TestJobInfoIO m_test_job_info_io;
};

TEST_F(JobInfoIOServerClientTest, Construction)
{
  // On construction, only static PVs that are always used will be trigger a first update.
  // This include JobState, OutputValue, Message and Log updates.
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(_)).Times(Exactly(0));
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, _)).Times(Exactly(0));
  EXPECT_CALL(m_test_job_info_io, VariableUpdated(_, _, _)).Times(Exactly(0));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(sup::sequencer::JobState::kInitial))
                                    .Times(Exactly(1));
  sup::dto::AnyValue empty_av{};
  EXPECT_CALL(m_test_job_info_io, PutValue(empty_av, "")).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, GetUserValue(_, _)).Times(Exactly(0));
  EXPECT_CALL(m_test_job_info_io, GetUserChoice(_, _)).Times(Exactly(0));
  EXPECT_CALL(m_test_job_info_io, Message("")).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(0, "")).Times(Exactly(1));

  const std::string job_prefix = "AVMgrServerClientConstruction";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
}

TEST_F(JobInfoIOServerClientTest, InitNrInstructions)
{
  // When InitNumberOfInstructions is called, there will be additional calls to
  // InitNumberOfInstructions and InstructionStateUpdated.
  unsigned nr_instr = 10u;
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  InstructionState initial_instr_state{ false, sup::sequencer::ExecutionStatus::NOT_STARTED };
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
  EXPECT_CALL(m_test_job_info_io, VariableUpdated(_, _, _)).Times(Exactly(0));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, GetUserValue(_, _)).Times(Exactly(0));
  EXPECT_CALL(m_test_job_info_io, GetUserChoice(_, _)).Times(Exactly(0));
  EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));

  const std::string job_prefix = "AVMgrServerClientConstruction";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
}
