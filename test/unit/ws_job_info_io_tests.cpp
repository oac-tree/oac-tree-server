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

#include <sup/oac-tree-server/base/input_request_server.h>

#include <sup/oac-tree/user_input_reply.h>

#include <sup/dto/anyvalue.h>

#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <future>
#include <memory>
#include <mutex>
#include <thread>

using namespace sup::oac_tree_server;

using sup::oac_tree::ExecutionStatus;
using sup::oac_tree::InstructionState;
using sup::oac_tree::JobState;

class WSJobInfoIOTest : public ::testing::Test
{
protected:
  WSJobInfoIOTest()
    : m_published_messages{}
    , m_mutex{}
    , m_request_server{std::make_shared<InputRequestServer>()}
    , m_job_info_io{[this](std::string msg) { RecordMessage(std::move(msg)); }, m_request_server,
                    std::chrono::milliseconds{50}}
  {}

  void RecordMessage(std::string msg)
  {
    std::lock_guard<std::mutex> lk{m_mutex};
    m_published_messages.push_back(std::move(msg));
  }

  std::size_t MessageCount()
  {
    std::lock_guard<std::mutex> lk{m_mutex};
    return m_published_messages.size();
  }

  std::vector<std::string> GetMessages()
  {
    std::lock_guard<std::mutex> lk{m_mutex};
    return m_published_messages;
  }

  // Block until the user input request for the given id has been published (and therefore
  // registered in the request server), so a reply can be delivered without racing the request.
  void WaitForRequestPublished()
  {
    while (MessageCount() == 0)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
  }

  std::vector<std::string> m_published_messages;
  std::mutex m_mutex;
  std::shared_ptr<InputRequestServer> m_request_server;
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

TEST_F(WSJobInfoIOTest, GetUserValuePublishesRequest)
{
  const sup::dto::AnyValue value{sup::dto::SignedInteger32Type, 0};
  const sup::dto::uint64 id{42u};

  auto future = std::async(std::launch::async, [this, id, value] {
    sup::dto::AnyValue local = value;
    return m_job_info_io.GetUserValue(id, local, "description");
  });
  WaitForRequestPublished();

  EXPECT_EQ(GetMessages().front(), CreateGetUserValueUpdateMessage(id, "description", value));

  // Unblock the waiting call so the test can finish.
  m_job_info_io.Interrupt(id);
  EXPECT_FALSE(future.get());
}

TEST_F(WSJobInfoIOTest, GetUserValueSuccess)
{
  const sup::dto::uint64 id{42u};
  const sup::dto::AnyValue provided{sup::dto::SignedInteger32Type, 7};

  sup::dto::AnyValue value{sup::dto::SignedInteger32Type, 0};
  auto future = std::async(std::launch::async, [this, id, &value] {
    return m_job_info_io.GetUserValue(id, value, "description");
  });
  WaitForRequestPublished();

  EXPECT_TRUE(m_request_server->SetClientReply(id, sup::oac_tree::CreateUserValueReply(true, provided)));
  EXPECT_TRUE(future.get());
  EXPECT_EQ(value, provided);
  // When finished, all clients are notified so they can dismiss any open dialog.
  EXPECT_EQ(GetMessages().back(), CreateUserInputCancelledMessage(id));
}

TEST_F(WSJobInfoIOTest, GetUserValueInterrupted)
{
  const sup::dto::uint64 id{99u};

  sup::dto::AnyValue value{sup::dto::SignedInteger32Type, 5};
  auto future = std::async(std::launch::async, [this, id, &value] {
    return m_job_info_io.GetUserValue(id, value, "description");
  });
  WaitForRequestPublished();

  m_job_info_io.Interrupt(id);
  EXPECT_FALSE(future.get());
  // A finished notification is sent even when the request was interrupted.
  EXPECT_EQ(GetMessages().back(), CreateUserInputCancelledMessage(id));
}

TEST_F(WSJobInfoIOTest, GetUserChoiceSuccess)
{
  const std::vector<std::string> options{"yes", "no", "maybe"};
  const sup::dto::AnyValue metadata{};
  const sup::dto::uint64 id{1u};

  auto future = std::async(std::launch::async, [this, id, options, metadata] {
    return m_job_info_io.GetUserChoice(id, options, metadata);
  });
  WaitForRequestPublished();

  EXPECT_TRUE(m_request_server->SetClientReply(id, sup::oac_tree::CreateUserChoiceReply(true, 2)));
  EXPECT_EQ(future.get(), 2);
  EXPECT_EQ(GetMessages().back(), CreateUserInputCancelledMessage(id));
}

TEST_F(WSJobInfoIOTest, GetUserChoiceInterrupted)
{
  const std::vector<std::string> options{"yes", "no"};
  const sup::dto::AnyValue metadata{};
  const sup::dto::uint64 id{1u};

  auto future = std::async(std::launch::async, [this, id, options, metadata] {
    return m_job_info_io.GetUserChoice(id, options, metadata);
  });
  WaitForRequestPublished();

  m_job_info_io.Interrupt(id);
  EXPECT_EQ(future.get(), -1);
  EXPECT_EQ(GetMessages().back(), CreateUserInputCancelledMessage(id));
}

TEST_F(WSJobInfoIOTest, GetUserValueResendsRequest)
{
  const sup::dto::uint64 id{11u};

  sup::dto::AnyValue value{sup::dto::SignedInteger32Type, 0};
  auto future = std::async(std::launch::async, [this, id, &value] {
    return m_job_info_io.GetUserValue(id, value, "description");
  });

  // The fixture uses a 50ms resend interval, so a second (re-published) message should appear.
  while (MessageCount() < 2)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds{5});
  }
  auto messages = GetMessages();
  // Every (re)published message carries the same req_id, i.e. it is identical.
  EXPECT_EQ(messages.front(), messages.back());

  // Provide a reply to stop the resender and unblock the call.
  const sup::dto::AnyValue provided{sup::dto::SignedInteger32Type, 3};
  m_request_server->SetClientReply(id, sup::oac_tree::CreateUserValueReply(true, provided));
  EXPECT_TRUE(future.get());
  EXPECT_EQ(value, provided);
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
