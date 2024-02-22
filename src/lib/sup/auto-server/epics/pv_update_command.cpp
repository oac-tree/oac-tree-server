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

#include "pv_update_command.h"

namespace sup
{
namespace auto_server
{

PVUpdateCommand::PVUpdateCommand(std::string channel, sup::dto::AnyValue value)
  : m_command_type{kUpdate}
  , m_channel{std::move(channel)}
  , m_value{std::move(value)}
{}

PVUpdateCommand::PVUpdateCommand(ExitTag tag)
  : m_command_type{kExit}
  , m_channel{}
  , m_value{}
{
  (void)tag;
}

PVUpdateCommand::~PVUpdateCommand() = default;

PVUpdateCommand::PVUpdateCommand(PVUpdateCommand&&) = default;

PVUpdateCommand& PVUpdateCommand::operator=(PVUpdateCommand&& other)
{
  m_command_type = other.m_command_type;
  m_channel = std::move(other.m_channel);
  m_value = std::move(other.m_value);
  return *this;
}

PVUpdateCommand::CommandType PVUpdateCommand::GetCommandType() const
{
  return m_command_type;
}

std::string& PVUpdateCommand::Channel()
{
  return m_channel;
}

sup::dto::AnyValue& PVUpdateCommand::Value()
{
  return m_value;
}

}  // namespace auto_server

}  // namespace sup
