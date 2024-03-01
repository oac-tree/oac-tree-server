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

#ifndef SUP_AUTO_SERVER_PV_UPDATE_COMMAND_H_
#define SUP_AUTO_SERVER_PV_UPDATE_COMMAND_H_

#include <sup/dto/anyvalue.h>

#include <string>

namespace sup
{

namespace auto_server
{

/**
 * @brief Class representing an update to a PV. It can also contain an exit command to be able to
 * terminate loops that are waiting for new commands.
 *
 * @note The class is move-only.
 */
class PVUpdateCommand
{
public:
  enum CommandType
  {
    kUpdate = 0,
    kExit
  };
  static PVUpdateCommand CreateChannelUpdate(const std::string& channel,
                                             const sup::dto::AnyValue& value);
  static PVUpdateCommand CreateExitCommand();

  ~PVUpdateCommand();

  // Move only
  PVUpdateCommand(PVUpdateCommand&& other);
  PVUpdateCommand& operator=(PVUpdateCommand&& other);

  CommandType GetCommandType() const;

  std::string& Channel();

  sup::dto::AnyValue& Value();

private:
  PVUpdateCommand(CommandType command_type, std::string channel, sup::dto::AnyValue value);
  CommandType m_command_type;
  std::string m_channel;
  sup::dto::AnyValue m_value;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_PV_UPDATE_COMMAND_H_
