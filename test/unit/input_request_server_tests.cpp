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

using namespace sup::auto_server;

class InputRequestServerTest : public ::testing::Test
{
protected:
  InputRequestServerTest() = default;
  virtual ~InputRequestServerTest() = default;
};

TEST_F(InputRequestServerTest, Construction)
{
  InputRequestServer server{};

  // There is no reply available just after construction:
  auto result = server.WaitForReply(0, 0.0);
  EXPECT_FALSE(result.first);
  EXPECT_TRUE(sup::dto::IsEmptyValue(result.second));

  // Setting the client reply fails for zero index, index that is not correct and for empty values:
  sup::dto::AnyValue reply{ sup::dto::UnsignedInteger32Type, 42u };
  EXPECT_FALSE(server.SetClientReply(0, reply));
  EXPECT_FALSE(server.SetClientReply(1, reply));
  EXPECT_FALSE(server.SetClientReply(1, {}));

  // First request gets index 1
  EXPECT_EQ(server.InitNewRequest(), 1u);
}

TEST_F(InputRequestServerTest, SingleThreaded)
{
  InputRequestServer server{};
  sup::dto::AnyValue reply{ sup::dto::UnsignedInteger32Type, 42u };

  // Test first request
  EXPECT_EQ(server.InitNewRequest(), 1u);
  auto result = server.WaitForReply(1u, 0.0);
  EXPECT_FALSE(result.first);
  EXPECT_TRUE(server.SetClientReply(1u, reply));
  // second set reply fails:
  EXPECT_FALSE(server.SetClientReply(1u, reply));
  // now there is a reply available
  result = server.WaitForReply(1u, 0.0);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, reply);

  // Test 'forgotten' request
  EXPECT_EQ(server.InitNewRequest(), 2u);
  EXPECT_EQ(server.InitNewRequest(), 3u);
  EXPECT_FALSE(server.SetClientReply(1u, reply));
  EXPECT_FALSE(server.SetClientReply(2u, reply));
  result = server.WaitForReply(3u, 0.0);
  EXPECT_FALSE(result.first);
  // Only this will succeed
  EXPECT_TRUE(server.SetClientReply(3u, reply));
  result = server.WaitForReply(3u, 0.0);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, reply);
}

TEST_F(InputRequestServerTest, MultiThreaded)
{
  InputRequestServer server{};
  sup::dto::AnyValue reply{ sup::dto::UnsignedInteger32Type, 42u };
  std::promise<void> ready;
  auto ready_future = ready.get_future();
  auto reply_setter = [&server, &ready, reply] {
    ready.set_value();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    server.SetClientReply(1u, reply);
  };

  // Test first request
  EXPECT_EQ(server.InitNewRequest(), 1u);
  auto client_thread = std::async(std::launch::async, reply_setter);
  ready_future.get();
  auto result = server.WaitForReply(1u, 0.1);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, reply);
}
