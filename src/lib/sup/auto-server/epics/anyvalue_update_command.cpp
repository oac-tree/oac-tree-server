/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVER
 *
 * Description   : SUP automation server
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

#include "anyvalue_update_command.h"

namespace sup
{
namespace auto_server
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

AnyValueUpdateCommand::~AnyValueUpdateCommand() = default;

AnyValueUpdateCommand::AnyValueUpdateCommand(AnyValueUpdateCommand&&) = default;

AnyValueUpdateCommand& AnyValueUpdateCommand::operator=(AnyValueUpdateCommand&& other)
{
  m_command_type = other.m_command_type;
  m_name = std::move(other.m_name);
  m_value = std::move(other.m_value);
  return *this;
}

AnyValueUpdateCommand::CommandType AnyValueUpdateCommand::GetCommandType() const
{
  return m_command_type;
}

std::string& AnyValueUpdateCommand::Name()
{
  return m_name;
}

sup::dto::AnyValue& AnyValueUpdateCommand::Value()
{
  return m_value;
}

AnyValueUpdateCommand::AnyValueUpdateCommand(CommandType command_type, std::string channel,
                                 sup::dto::AnyValue value)
  : m_command_type{command_type}
  , m_name{std::move(channel)}
  , m_value{std::move(value)}
{}

}  // namespace auto_server

}  // namespace sup
