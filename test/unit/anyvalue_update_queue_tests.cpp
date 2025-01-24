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

#include <sup/oac-tree-server/epics/anyvalue_update_queue.h>

#include <gtest/gtest.h>

#include <atomic>
#include <future>
#include <thread>

using namespace sup::auto_server;

class AnyValueUpdateQueueTest : public ::testing::Test
{
protected:
  AnyValueUpdateQueueTest() = default;
  virtual ~AnyValueUpdateQueueTest() = default;
};

TEST_F(AnyValueUpdateQueueTest, PushPop)
{
  // Create queue with 3 update commands and an exit command
  AnyValueUpdateQueue update_queue{};
  const std::string var_name = "my_var";
  sup::dto::AnyValue var_val_1{ sup::dto::UnsignedInteger16Type, 1u };
  sup::dto::AnyValue var_val_2{ sup::dto::UnsignedInteger16Type, 2u };
  sup::dto::AnyValue var_val_3{ sup::dto::UnsignedInteger16Type, 3u };
  update_queue.Push(var_name, var_val_1);
  update_queue.Push(var_name, var_val_2);
  update_queue.Push(var_name, var_val_3);
  update_queue.PushExit();

  // Check popped commands
  update_queue.WaitForNonEmpty();
  auto commands = update_queue.PopCommands();
  ASSERT_EQ(commands.size(), 4);
  EXPECT_EQ(commands.front().GetCommandType(), AnyValueUpdateCommand::CommandType::kUpdate);
  EXPECT_EQ(commands.front().Name(), var_name);
  EXPECT_EQ(commands.front().Value(), var_val_1);
  commands.pop_front();
  EXPECT_EQ(commands.front().GetCommandType(), AnyValueUpdateCommand::CommandType::kUpdate);
  EXPECT_EQ(commands.front().Name(), var_name);
  EXPECT_EQ(commands.front().Value(), var_val_2);
  commands.pop_front();
  EXPECT_EQ(commands.front().GetCommandType(), AnyValueUpdateCommand::CommandType::kUpdate);
  EXPECT_EQ(commands.front().Name(), var_name);
  EXPECT_EQ(commands.front().Value(), var_val_3);
  commands.pop_front();
  EXPECT_EQ(commands.front().GetCommandType(), AnyValueUpdateCommand::CommandType::kExit);
  EXPECT_EQ(commands.front().Name(), "");
  EXPECT_EQ(commands.front().Value(), sup::dto::AnyValue{});
}

TEST_F(AnyValueUpdateQueueTest, WaitForNonEmpty)
{
  // Create queue with 3 update commands and an exit command
  AnyValueUpdateQueue update_queue{};
  const std::string var_name = "my_var";
  sup::dto::AnyValue var_val{ sup::dto::UnsignedInteger16Type, 1u };

  // Launch thread that waits for non empty queue
  std::atomic_bool is_finished{false};
  std::promise<void> ready;
  auto ready_future = ready.get_future();
  auto wait_func = [&update_queue, &ready, &is_finished](){
    ready.set_value();
    update_queue.WaitForNonEmpty();
    is_finished.store(true);
  };
  auto wait_future = std::async(std::launch::async, wait_func);
  ready_future.get();

  // Wait a bit, and notice that the thread is still waiting:
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_FALSE(is_finished.load());

  // Push a value and the thread will finish
  update_queue.Push(var_name, var_val);
  wait_future.get();
  EXPECT_TRUE(is_finished.load());
}
