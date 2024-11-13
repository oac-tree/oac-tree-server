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

#include <sup/auto-server/input_request_server.h>

#include <future>

#include <gtest/gtest.h>
#include <thread>

using namespace sup::auto_server;

using sup::sequencer::kInvalidUserInputReply;
using sup::sequencer::InputRequestType;
using sup::sequencer::UserInputReply;

class InputRequestServerTest : public ::testing::Test
{
protected:
  InputRequestServerTest() = default;
  virtual ~InputRequestServerTest() = default;
};

TEST_F(InputRequestServerTest, Construction)
{
  InputRequestServer server{};

  // Id zero will immediately return an invalid reply:
  auto result = server.WaitForReply(0);
  EXPECT_FALSE(result.first);
  EXPECT_EQ(result.second, kInvalidUserInputReply);

  // Setting the client reply fails for zero index, index that is not correct and invalid replies:
  UserInputReply reply{ InputRequestType::kUserChoice, true, { sup::dto::SignedInteger32Type, 0 }};
  EXPECT_FALSE(server.SetClientReply(0, reply));
  EXPECT_FALSE(server.SetClientReply(1, reply));
  EXPECT_FALSE(server.SetClientReply(1, kInvalidUserInputReply));
}

TEST_F(InputRequestServerTest, SingleThreaded)
{
  InputRequestServer server{};
  UserInputReply reply{ InputRequestType::kUserValue, true,
                        { sup::dto::UnsignedInteger32Type, 42u }};

  // Test first request
  server.InitNewRequest(1u);
  EXPECT_TRUE(server.SetClientReply(1u, reply));
  // second set reply fails:
  EXPECT_FALSE(server.SetClientReply(1u, reply));
  // now there is a reply available
  auto result = server.WaitForReply(1u);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, reply);

  // Test 'forgotten' request
  server.InitNewRequest(2u);
  server.InitNewRequest(3u);
  EXPECT_FALSE(server.SetClientReply(1u, reply));
  EXPECT_FALSE(server.SetClientReply(2u, reply));
  // Only this will succeed
  EXPECT_TRUE(server.SetClientReply(3u, reply));
  result = server.WaitForReply(3u);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, reply);
}

TEST_F(InputRequestServerTest, MultiThreaded)
{
  InputRequestServer server{};
  UserInputReply reply{ InputRequestType::kUserValue, true,
                        { sup::dto::UnsignedInteger32Type, 42u }};
  std::promise<void> ready;
  auto ready_future = ready.get_future();
  auto reply_setter = [&server, &ready, reply] {
    ready.set_value();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    server.SetClientReply(1u, reply);
  };

  // Verify WaitForReply
  server.InitNewRequest(1u);
  auto client_thread = std::async(std::launch::async, reply_setter);
  ready_future.get();
  auto result = server.WaitForReply(1u);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, reply);
}
