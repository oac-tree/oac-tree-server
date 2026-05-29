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
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include <sup/oac-tree-server/websocket/ws_job_info_io.h>
#include <sup/oac-tree-server/websocket/ws_message.h>

#include <sup/dto/anyvalue.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree_server;

using sup::oac_tree::ExecutionStatus;
using sup::oac_tree::InstructionState;
using sup::oac_tree::JobState;

class WSJobInfoIOTest : public ::testing::Test
{
protected:
  WSJobInfoIOTest()
    : m_job_info_io{[this](std::string msg) { m_published_messages.push_back(std::move(msg)); }}
  {}

  std::vector<std::string> m_published_messages;
  WSJobInfoIO m_job_info_io;
};

TEST_F(WSJobInfoIOTest, InitNumberOfInstructions)
{
  m_job_info_io.InitNumberOfInstructions(10u);

  EXPECT_TRUE(m_published_messages.empty());
}

TEST_F(WSJobInfoIOTest, InstructionStateUpdated)
{
  InstructionState state{true, ExecutionStatus::SUCCESS};

  m_job_info_io.InstructionStateUpdated(3u, state);

  ASSERT_EQ(m_published_messages.size(), 1u);
  EXPECT_EQ(m_published_messages[0], CreateInstructionStateUpdateMessage(3u, state));
}

TEST_F(WSJobInfoIOTest, BreakpointInstructionUpdated)
{
  m_job_info_io.BreakpointInstructionUpdated(5u);

  ASSERT_EQ(m_published_messages.size(), 1u);
  EXPECT_EQ(m_published_messages[0], CreateBreakpointInstructionUpdateMessage(5u));
}

TEST_F(WSJobInfoIOTest, VariableUpdated)
{
  const sup::dto::AnyValue value{sup::dto::Float64Type, 3.14};

  m_job_info_io.VariableUpdated(2u, value, true);

  ASSERT_EQ(m_published_messages.size(), 1u);
  EXPECT_EQ(m_published_messages[0], CreateVariableStateUpdateMessage(2u, true, value));
}

TEST_F(WSJobInfoIOTest, JobStateUpdated)
{
  m_job_info_io.JobStateUpdated(JobState::kRunning);

  ASSERT_EQ(m_published_messages.size(), 1u);
  EXPECT_EQ(m_published_messages[0], CreateJobStateUpdateMessage(JobState::kRunning));
}

TEST_F(WSJobInfoIOTest, PutValue)
{
  const sup::dto::AnyValue value{sup::dto::BooleanType, true};

  m_job_info_io.PutValue(value, "description");

  ASSERT_EQ(m_published_messages.size(), 1u);
  EXPECT_EQ(m_published_messages[0], CreatePutValueUpdateMessage("description", value));
}

TEST_F(WSJobInfoIOTest, GetUserValue)
{
  sup::dto::AnyValue value{};

  bool result = m_job_info_io.GetUserValue(42u, value, "description");

  EXPECT_FALSE(result);
  EXPECT_TRUE(m_published_messages.empty());
}

TEST_F(WSJobInfoIOTest, GetUserChoice)
{
  const std::vector<std::string> options{"yes", "no"};
  const sup::dto::AnyValue metadata{};

  int result = m_job_info_io.GetUserChoice(1u, options, metadata);

  EXPECT_EQ(result, -1);
  EXPECT_TRUE(m_published_messages.empty());
}

TEST_F(WSJobInfoIOTest, Interrupt)
{
  m_job_info_io.Interrupt(7u);

  EXPECT_TRUE(m_published_messages.empty());
}

TEST_F(WSJobInfoIOTest, Message)
{
  m_job_info_io.Message("hello world");

  ASSERT_EQ(m_published_messages.size(), 1u);
  EXPECT_EQ(m_published_messages[0], CreateTextMessageUpdateMessage("hello world"));
}

TEST_F(WSJobInfoIOTest, Log)
{
  m_job_info_io.Log(3, "warning message");

  ASSERT_EQ(m_published_messages.size(), 1u);
  EXPECT_EQ(m_published_messages[0], CreateLogUpdateMessage(3, "warning message"));
}

TEST_F(WSJobInfoIOTest, ProcedureTicked)
{
  m_job_info_io.ProcedureTicked();

  EXPECT_TRUE(m_published_messages.empty());
}
