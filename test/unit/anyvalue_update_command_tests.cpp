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

#include <sup/oac-tree-server/epics/anyvalue_update_command.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class AnyValueUpdateCommandTest : public ::testing::Test
{
protected:
  AnyValueUpdateCommandTest() = default;
  virtual ~AnyValueUpdateCommandTest() = default;
};

TEST_F(AnyValueUpdateCommandTest, CreateCommands)
{
  // Update command
  const std::string var_name = "my_var";
  sup::dto::AnyValue var_val{ sup::dto::Float64Type, 3.14 };
  auto update_command = AnyValueUpdateCommand::CreateValueUpdate(var_name, var_val);
  EXPECT_EQ(update_command.GetCommandType(), AnyValueUpdateCommand::CommandType::kUpdate);
  EXPECT_EQ(update_command.Name(), var_name);
  EXPECT_EQ(update_command.Value(), var_val);

  // Exit command
  auto exit_command = AnyValueUpdateCommand::CreateExitCommand();
  EXPECT_EQ(exit_command.GetCommandType(), AnyValueUpdateCommand::CommandType::kExit);
  EXPECT_EQ(exit_command.Name(), "");
  EXPECT_EQ(exit_command.Value(), sup::dto::AnyValue{});
}

TEST_F(AnyValueUpdateCommandTest, Move)
{
  // Original command
  const std::string var_name = "my_var";
  sup::dto::AnyValue var_val{ sup::dto::Float64Type, 3.14 };
  auto update_command = AnyValueUpdateCommand::CreateValueUpdate(var_name, var_val);
  EXPECT_EQ(update_command.GetCommandType(), AnyValueUpdateCommand::CommandType::kUpdate);
  EXPECT_EQ(update_command.Name(), var_name);
  EXPECT_EQ(update_command.Value(), var_val);

  // Move ctor
  AnyValueUpdateCommand move_constructed{std::move(update_command)};
  EXPECT_EQ(move_constructed.GetCommandType(), AnyValueUpdateCommand::CommandType::kUpdate);
  EXPECT_EQ(move_constructed.Name(), var_name);
  EXPECT_EQ(move_constructed.Value(), var_val);

  // Move assignment
  auto move_assigned = AnyValueUpdateCommand::CreateExitCommand();
  move_assigned = std::move(move_constructed);
  EXPECT_EQ(move_assigned.GetCommandType(), AnyValueUpdateCommand::CommandType::kUpdate);
  EXPECT_EQ(move_assigned.Name(), var_name);
  EXPECT_EQ(move_assigned.Value(), var_val);
}
