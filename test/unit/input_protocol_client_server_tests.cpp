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

#include <sup/auto-server/input_protocol_client.h>
#include <sup/auto-server/input_protocol_server.h>

#include <gtest/gtest.h>

#include <atomic>
#include <future>

using namespace sup::auto_server;

class InputProtocolClientServerTest : public ::testing::Test
{
protected:
  InputProtocolClientServerTest();
  virtual ~InputProtocolClientServerTest() = default;

  InputProtocolServer m_server;
  InputProtocolClient m_client;
};

TEST_F(InputProtocolClientServerTest, SingleThreaded)
{
  auto reply = sup::sequencer::CreateUserChoiceReply(true, 42);
  sup::dto::uint64 id{77};
  m_server.InitNewRequest(id);
  m_client.SetClientReply(id, reply);
  auto reply_at_server = m_server.WaitForReply(id);
  EXPECT_TRUE(reply_at_server.first);
  EXPECT_EQ(reply_at_server.second, reply);
}

TEST_F(InputProtocolClientServerTest, MultiThreaded)
{
  auto reply = sup::sequencer::CreateUserChoiceReply(true, 42);
  sup::dto::uint64 id{77};
  m_server.InitNewRequest(id);
  auto client_func = [this, id, reply]() {
    m_client.SetClientReply(id, reply);
  };
  auto client_future = std::async(std::launch::async, client_func);
  auto reply_at_server = m_server.WaitForReply(id);
  EXPECT_TRUE(reply_at_server.first);
  EXPECT_EQ(reply_at_server.second, reply);
}

TEST_F(InputProtocolClientServerTest, MultiThreadedInterrupted)
{
  auto reply = sup::sequencer::CreateUserChoiceReply(true, 42);
  sup::dto::uint64 id{77};
  m_server.InitNewRequest(id);
  std::atomic_bool halt{false};
  auto client_func = [this, id, reply, &halt]() {
    while (!halt)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  };
  auto client_future = std::async(std::launch::async, client_func);
  m_server.Interrupt(id);
  auto reply_at_server = m_server.WaitForReply(id);
  EXPECT_FALSE(reply_at_server.first);
  EXPECT_EQ(reply_at_server.second, sup::sequencer::kInvalidUserInputReply);
  halt.store(true);
}

InputProtocolClientServerTest::InputProtocolClientServerTest()
  : m_server{}
  , m_client{m_server}
{}