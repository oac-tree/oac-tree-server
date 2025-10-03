/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : oac-tree server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
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

#include "anyvalue_update_command.h"

namespace sup
{
namespace oac_tree_server
{

AnyValueUpdateCommand AnyValueUpdateCommand::CreateValueUpdate(const std::string& channel,
                                                               const sup::dto::AnyValue& value)
{
  return AnyValueUpdateCommand(kUpdate, channel, value);
}

AnyValueUpdateCommand AnyValueUpdateCommand::CreateExitCommand()
{
  return AnyValueUpdateCommand(kExit, {}, {});
}

AnyValueUpdateCommand::~AnyValueUpdateCommand() noexcept = default;

AnyValueUpdateCommand::AnyValueUpdateCommand(AnyValueUpdateCommand&&) noexcept = default;

AnyValueUpdateCommand& AnyValueUpdateCommand::operator=(AnyValueUpdateCommand&& other) noexcept
{
  m_command_type = other.m_command_type;
  m_channel = std::move(other.m_channel);
  m_value = std::move(other.m_value);
  return *this;
}

AnyValueUpdateCommand::CommandType AnyValueUpdateCommand::GetCommandType() const
{
  return m_command_type;
}

std::string& AnyValueUpdateCommand::Name()
{
  return m_channel;
}

sup::dto::AnyValue& AnyValueUpdateCommand::Value()
{
  return m_value;
}

AnyValueUpdateCommand::AnyValueUpdateCommand(CommandType command_type, std::string channel,
                                 sup::dto::AnyValue value)
  : m_command_type{command_type}
  , m_channel{std::move(channel)}
  , m_value{std::move(value)}
{}

}  // namespace oac_tree_server

}  // namespace sup
