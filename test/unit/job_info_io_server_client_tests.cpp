/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : Unit test code
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/oac-tree-server/client_anyvalue_manager.h>
#include <sup/oac-tree-server/server_job_info_io.h>

#include "unit_test_helper.h"

#include <sup/oac-tree/log_severity.h>

#include <gtest/gtest.h>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Exactly;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetArgReferee;

using namespace sup::oac_tree_server;

using sup::oac_tree::InstructionState;

class JobInfoIOServerClientTest : public ::testing::Test
{
protected:
  JobInfoIOServerClientTest() = default;
  virtual ~JobInfoIOServerClientTest() = default;

  using StrictMockJobInfoIO = ::testing::StrictMock<UnitTestHelper::MockJobInfoIO>;
  StrictMockJobInfoIO m_test_job_info_io;
};

TEST_F(JobInfoIOServerClientTest, Construction)
{
  // On construction, only static PVs that are always used will trigger a first update.
  // These include JobState, OutputValue, Message and Log updates.
  // Not that no variable state is updated, as they contain no meaningful value
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(sup::oac_tree::JobState::kInitial))
                                    .Times(Exactly(1));
  sup::dto::AnyValue empty_av{};
  EXPECT_CALL(m_test_job_info_io, PutValue(empty_av, "")).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Message("")).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(0, "")).Times(Exactly(1));

  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
}

TEST_F(JobInfoIOServerClientTest, InitNrInstructions)
{
  // When InitNumberOfInstructions is called, there will be additional calls to
  // InitNumberOfInstructions and InstructionStateUpdated.
  unsigned nr_instr = 10u;
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  InstructionState initial_instr_state{ false, sup::oac_tree::ExecutionStatus::NOT_STARTED };
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));

  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
}

TEST_F(JobInfoIOServerClientTest, InstructionStateUpdated)
{
  // When InstructionStateUpdated is called, there will be an additional call to
  // InstructionStateUpdated of the mock class.
  unsigned nr_instr = 10u;
  InstructionState initial_instr_state{ false, sup::oac_tree::ExecutionStatus::NOT_STARTED };
  InstructionState instr_state{ true, sup::oac_tree::ExecutionStatus::NOT_FINISHED };
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));
  {
    InSequence seq;
    EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
    EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(5, instr_state)).Times(Exactly(1));
  }
  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
  server_job_info_io.InstructionStateUpdated(5, instr_state);
}

TEST_F(JobInfoIOServerClientTest, VariableUpdated)
{
  // When VariableUpdated is called, there will be an additional call to
  // VariableUpdated of the mock class.
  unsigned nr_instr = 10u;
  sup::dto::AnyValue var_value{ sup::dto::UnsignedInteger64Type, 42u };
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  InstructionState initial_instr_state{ false, sup::oac_tree::ExecutionStatus::NOT_STARTED };
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
  EXPECT_CALL(m_test_job_info_io, VariableUpdated(2, var_value, true)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));

  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
  server_job_info_io.VariableUpdated(2, var_value, true);
}

TEST_F(JobInfoIOServerClientTest, JobStateUpdated)
{
  // When JobStateUpdated is called, there will be an additional call to
  // JobStateUpdated of the mock class.
  unsigned nr_instr = 10u;
  sup::oac_tree::JobState initial_job_state = sup::oac_tree::JobState::kInitial;
  sup::oac_tree::JobState new_job_state = sup::oac_tree::JobState::kSucceeded;
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  InstructionState initial_instr_state{ false, sup::oac_tree::ExecutionStatus::NOT_STARTED };
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
  EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));
  {
    InSequence seq;
    EXPECT_CALL(m_test_job_info_io, JobStateUpdated(initial_job_state)).Times(Exactly(1));
    EXPECT_CALL(m_test_job_info_io, JobStateUpdated(new_job_state)).Times(Exactly(1));
  }

  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
  server_job_info_io.JobStateUpdated(new_job_state);
}

TEST_F(JobInfoIOServerClientTest, PutValue)
{
  // When PutValue is called, there will be an additional call to
  // PutValue of the mock class.
  unsigned nr_instr = 10u;
  const std::string description = "Look at this value!";
  sup::dto::AnyValue new_value{ sup::dto::UnsignedInteger64Type, 42u };
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  InstructionState initial_instr_state{ false, sup::oac_tree::ExecutionStatus::NOT_STARTED };
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));
  {
    InSequence seq;
    EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
    EXPECT_CALL(m_test_job_info_io, PutValue(new_value, description)).Times(Exactly(1));
  }

  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
  server_job_info_io.PutValue(new_value, description);
}

TEST_F(JobInfoIOServerClientTest, Message)
{
  // When Message is called, there will be an additional call to
  // Message of the mock class.
  unsigned nr_instr = 10u;
  const std::string message = "This is an important message!";
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  InstructionState initial_instr_state{ false, sup::oac_tree::ExecutionStatus::NOT_STARTED };
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));
  {
    InSequence seq;
    EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
    EXPECT_CALL(m_test_job_info_io, Message(message)).Times(Exactly(1));
  }

  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
  server_job_info_io.Message(message);
}

TEST_F(JobInfoIOServerClientTest, Log)
{
  // When Log is called, there will be an additional call to
  // Log of the mock class.
  unsigned nr_instr = 10u;
  auto severity = sup::oac_tree::log::SUP_SEQ_LOG_CRIT;
  const std::string message = "A critical issue was detected!";
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  InstructionState initial_instr_state{ false, sup::oac_tree::ExecutionStatus::NOT_STARTED };
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
  {
    InSequence seq;
    EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));
    EXPECT_CALL(m_test_job_info_io, Log(severity, message)).Times(Exactly(1));
  }

  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
  server_job_info_io.Log(severity, message);
}

TEST_F(JobInfoIOServerClientTest, GetUserValue)
{
  // When GetUserValue is called, there will be an additional call to
  // GetUserValue of the mock class.
  unsigned nr_instr = 10u;
  const std::string description = "Please provide me some value";
  sup::dto::AnyValue user_val{ sup::dto::UnsignedInteger64Type, 42u };
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  InstructionState initial_instr_state{ false, sup::oac_tree::ExecutionStatus::NOT_STARTED };
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, GetUserValue(1u, _, description)).Times(Exactly(1)).WillOnce(
    DoAll(SetArgReferee<1>(user_val), Return(true)));
  EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));

  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
  sup::dto::AnyValue response;
  EXPECT_TRUE(server_job_info_io.GetUserValue(1u, response, description));
  EXPECT_EQ(response, user_val);
}

TEST_F(JobInfoIOServerClientTest, GetUserChoice)
{
  // When GetUserChoice is called, there will be an additional call to
  // GetUserChoice of the mock class.
  unsigned nr_instr = 10u;
  const std::vector<std::string> choices{ "vanilla", "strawberry", "chocolate"};
  sup::dto::AnyValue metadata{ sup::dto::StringType, "Choose your favorite flavour"};
  int choice = 2;
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  InstructionState initial_instr_state{ false, sup::oac_tree::ExecutionStatus::NOT_STARTED };
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, GetUserChoice(1u, choices, metadata)).Times(Exactly(1))
    .WillOnce(Return(choice));
  EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));

  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
  EXPECT_EQ(server_job_info_io.GetUserChoice(1u, choices, metadata), choice);
}

TEST_F(JobInfoIOServerClientTest, ProcedureTicked)
{
  // Procedure ticks are not forwarded over the network!
  unsigned nr_instr = 10u;
  EXPECT_CALL(m_test_job_info_io, InitNumberOfInstructions(10)).Times(Exactly(1));
  InstructionState initial_instr_state{ false, sup::oac_tree::ExecutionStatus::NOT_STARTED };
  EXPECT_CALL(m_test_job_info_io, InstructionStateUpdated(_, initial_instr_state)).Times(Exactly(nr_instr));
  EXPECT_CALL(m_test_job_info_io, JobStateUpdated(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, PutValue(_, _)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Message(_)).Times(Exactly(1));
  EXPECT_CALL(m_test_job_info_io, Log(_, _)).Times(Exactly(1));

  const std::string job_prefix = "JobInfoIOClientServerTest";
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  ServerJobInfoIO server_job_info_io{job_prefix, 5, client_av_mgr};
  server_job_info_io.InitNumberOfInstructions(nr_instr);
  server_job_info_io.ProcedureTicked();
}
