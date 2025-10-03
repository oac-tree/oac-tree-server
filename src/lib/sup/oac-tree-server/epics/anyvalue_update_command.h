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

#ifndef SUP_OAC_TREE_SERVER_ANYVALUE_UPDATE_COMMAND_H_
#define SUP_OAC_TREE_SERVER_ANYVALUE_UPDATE_COMMAND_H_

#include <sup/dto/anyvalue.h>
#include <sup/dto/basic_scalar_types.h>

#include <string>

namespace sup
{

namespace oac_tree_server
{

/**
 * @brief Class representing an update to a AnyValue. It can also contain an exit command to be able
 * to terminate loops that are waiting for new commands.
 *
 * @note The class is move-only.
 */
class AnyValueUpdateCommand
{
public:
  enum CommandType : dto::uint32
  {
    kUpdate = 0,
    kExit
  };
  static AnyValueUpdateCommand CreateValueUpdate(const std::string& channel,
                                                 const sup::dto::AnyValue& value);
  static AnyValueUpdateCommand CreateExitCommand();

  ~AnyValueUpdateCommand();

  // Move only
  AnyValueUpdateCommand(AnyValueUpdateCommand&& other);
  AnyValueUpdateCommand& operator=(AnyValueUpdateCommand&& other);

  CommandType GetCommandType() const;

  std::string& Name();

  sup::dto::AnyValue& Value();

private:
  AnyValueUpdateCommand(CommandType command_type, std::string channel, sup::dto::AnyValue value);
  CommandType m_command_type;
  std::string m_channel;
  sup::dto::AnyValue m_value;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_ANYVALUE_UPDATE_COMMAND_H_
