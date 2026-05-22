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

#include <sup/oac-tree-server/json_message_handler.h>
#include <sup/oac-tree-server/ws_message.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

using ::testing::Exactly;

using namespace sup::oac_tree_server;

using sup::oac_tree::ExecutionStatus;
using sup::oac_tree::InstructionState;
using sup::oac_tree::JobState;

class UpdateMessageHandlerTest : public ::testing::Test
{
protected:
  using StrictMockJobInfoIO = ::testing::StrictMock<UnitTestHelper::MockJobInfoIO>;
  StrictMockJobInfoIO m_mock_io;
};

// ---------------------------------------------------------------------------
// Happy-path: one test per concrete handler
// ---------------------------------------------------------------------------

TEST_F(UpdateMessageHandlerTest, HandleMessage_JobStateUpdate)
{
  const auto state = JobState::kFailed;
  EXPECT_CALL(m_mock_io, JobStateUpdated(state)).Times(Exactly(1));
  HandleUpdateMessage(CreateJobStateUpdateMessage(state), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_BreakpointUpdate)
{
  EXPECT_CALL(m_mock_io, BreakpointInstructionUpdated(42u)).Times(Exactly(1));
  HandleUpdateMessage(CreateBreakpointInstructionUpdateMessage(42u), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_InstructionStateUpdate)
{
  InstructionState state{true, ExecutionStatus::SUCCESS};
  EXPECT_CALL(m_mock_io, InstructionStateUpdated(8u, state)).Times(Exactly(1));
  HandleUpdateMessage(CreateInstructionStateUpdateMessage(8u, true, state), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_VariableStateUpdate)
{
  const sup::dto::AnyValue value{sup::dto::Float64Type, 6.5};
  EXPECT_CALL(m_mock_io, VariableUpdated(7u, value, true)).Times(Exactly(1));
  HandleUpdateMessage(CreateVariableStateUpdateMessage(7u, true, value), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_PutValueUpdate)
{
  const sup::dto::AnyValue value{sup::dto::BooleanType, true};
  EXPECT_CALL(m_mock_io, PutValue(value, std::string("desc"))).Times(Exactly(1));
  HandleUpdateMessage(CreatePutValueUpdateMessage("desc", value), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_TextMessageUpdate)
{
  EXPECT_CALL(m_mock_io, Message(std::string("hello"))).Times(Exactly(1));
  HandleUpdateMessage(CreateTextMessageUpdateMessage("hello"), m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_LogUpdate)
{
  EXPECT_CALL(m_mock_io, Log(5, std::string("warn"))).Times(Exactly(1));
  HandleUpdateMessage(CreateLogUpdateMessage(5, "warn"), m_mock_io);
}

// ---------------------------------------------------------------------------
// Robustness: StrictMock ensures no IJobInfoIO method is called
// ---------------------------------------------------------------------------

TEST_F(UpdateMessageHandlerTest, HandleMessage_InvalidJson)
{
  HandleUpdateMessage("not json", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_EmptyString)
{
  HandleUpdateMessage("", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_MissingUpdateField)
{
  HandleUpdateMessage(R"({"state": 0})", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_UnknownUpdateType)
{
  HandleUpdateMessage(R"({"update": 99})", m_mock_io);
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_JobState_MissingState)
{
  HandleUpdateMessage(R"({"update": 0})", m_mock_io);
}
