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

#include <sup/oac-tree-server/update_message_handler.h>
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
  UpdateMessageHandler m_handler{m_mock_io};
};

// ---------------------------------------------------------------------------
// Happy-path: one test per concrete handler
// ---------------------------------------------------------------------------

TEST_F(UpdateMessageHandlerTest, HandleMessage_JobStateUpdate)
{
  const auto state = JobState::kFailed;
  EXPECT_CALL(m_mock_io, JobStateUpdated(state)).Times(Exactly(1));
  m_handler.HandleMessage(CreateJobStateUpdateMessage(state));
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_BreakpointUpdate)
{
  EXPECT_CALL(m_mock_io, BreakpointInstructionUpdated(42u)).Times(Exactly(1));
  m_handler.HandleMessage(CreateBreakpointInstructionUpdateMessage(42u));
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_InstructionStateUpdate)
{
  InstructionState state{true, ExecutionStatus::SUCCESS};
  EXPECT_CALL(m_mock_io, InstructionStateUpdated(8u, state)).Times(Exactly(1));
  m_handler.HandleMessage(CreateInstructionStateUpdateMessage(8u, true, state));
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_VariableStateUpdate)
{
  const sup::dto::AnyValue value{sup::dto::Float64Type, 6.5};
  EXPECT_CALL(m_mock_io, VariableUpdated(7u, value, true)).Times(Exactly(1));
  m_handler.HandleMessage(CreateVariableStateUpdateMessage(7u, true, value));
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_PutValueUpdate)
{
  const sup::dto::AnyValue value{sup::dto::BooleanType, true};
  EXPECT_CALL(m_mock_io, PutValue(value, std::string("desc"))).Times(Exactly(1));
  m_handler.HandleMessage(CreatePutValueUpdateMessage("desc", value));
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_TextMessageUpdate)
{
  EXPECT_CALL(m_mock_io, Message(std::string("hello"))).Times(Exactly(1));
  m_handler.HandleMessage(CreateTextMessageUpdateMessage("hello"));
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_LogUpdate)
{
  EXPECT_CALL(m_mock_io, Log(5, std::string("warn"))).Times(Exactly(1));
  m_handler.HandleMessage(CreateLogUpdateMessage(5, "warn"));
}

// ---------------------------------------------------------------------------
// Robustness: StrictMock ensures no IJobInfoIO method is called
// ---------------------------------------------------------------------------

TEST_F(UpdateMessageHandlerTest, HandleMessage_InvalidJson)
{
  m_handler.HandleMessage("not json");
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_EmptyString)
{
  m_handler.HandleMessage("");
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_MissingUpdateField)
{
  m_handler.HandleMessage(R"({"state": 0})");
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_UnknownUpdateType)
{
  m_handler.HandleMessage(R"({"update": 99})");
}

TEST_F(UpdateMessageHandlerTest, HandleMessage_JobState_MissingState)
{
  m_handler.HandleMessage(R"({"update": 0})");
}
