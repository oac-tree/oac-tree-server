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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/oac-tree-server/epics/epics_input_client.h>
#include <sup/oac-tree-server/epics/epics_input_server.h>

#include <gtest/gtest.h>

#include <atomic>
#include <future>
#include <thread>

using namespace sup::auto_server;

class EPICSInputClientServerTest : public ::testing::Test
{
protected:
  EPICSInputClientServerTest() = default;
  virtual ~EPICSInputClientServerTest() = default;
};

TEST_F(EPICSInputClientServerTest, SingleThreaded)
{
  const std::string server_name = "EPICSInputClientServer_Tests::SingleThreaded";
  auto server = EPICSInputServer{server_name};
  auto client = EPICSInputClient{server_name};
  auto reply = sup::oac_tree::CreateUserChoiceReply(true, 42);
  sup::dto::uint64 id{77};
  server.InitNewRequest(id);
  client.SetClientReply(id, reply);
  auto [retrieved, value] = server.WaitForReply(id);
  EXPECT_TRUE(retrieved);
  EXPECT_EQ(value, reply);
}

TEST_F(EPICSInputClientServerTest, MultiThreaded)
{
  const std::string server_name = "EPICSInputClientServer_Tests::MultiThreaded";
  auto server = EPICSInputServer{server_name};
  auto client = EPICSInputClient{server_name};
  auto reply = sup::oac_tree::CreateUserChoiceReply(true, 42);
  sup::dto::uint64 id{77};
  server.InitNewRequest(id);
  auto client_func = [&client, id, reply]() {
    client.SetClientReply(id, reply);
  };
  auto client_future = std::async(std::launch::async, client_func);
  auto [retrieved, value] = server.WaitForReply(id);
  EXPECT_TRUE(retrieved);
  EXPECT_EQ(value, reply);
}

TEST_F(EPICSInputClientServerTest, MultiThreadedInterrupted)
{
  const std::string server_name = "EPICSInputClientServer_Tests::MultiThreadedInterrupted";
  auto server = EPICSInputServer{server_name};
  auto client = EPICSInputClient{server_name};
  auto reply = sup::oac_tree::CreateUserChoiceReply(true, 42);
  sup::dto::uint64 id{77};
  server.InitNewRequest(id);
  std::atomic_bool halt{false};
  auto client_func = [&halt]() {
    while (!halt)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  };
  auto client_future = std::async(std::launch::async, client_func);
  server.Interrupt(id);
  auto [retrieved, value] = server.WaitForReply(id);
  EXPECT_FALSE(retrieved);
  EXPECT_EQ(value, sup::oac_tree::kInvalidUserInputReply);
  halt.store(true);
}
