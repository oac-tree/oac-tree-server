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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/oac-tree-server/exceptions.h>

namespace sup
{
namespace auto_server
{

MessageException::MessageException(std::string message)
  : m_message{std::move(message)}
{}

const char* MessageException::what() const noexcept
{
  return m_message.c_str();
}

InvalidOperationException::InvalidOperationException(const std::string& message)
  : MessageException{message}
{}

}  // namespace auto_server

}  // namespace sup
