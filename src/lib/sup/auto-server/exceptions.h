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

#ifndef SUP_AUTO_SERVER_EXCEPTIONS_H_
#define SUP_AUTO_SERVER_EXCEPTIONS_H_

#include <exception>
#include <string>

namespace sup
{
namespace auto_server
{

  /**
 * @brief Base Exception class with message.
 */
class MessageException : public std::exception
{
public:
  explicit MessageException(std::string message);
  const char* what() const noexcept override;
private:
  std::string m_message;
};

/**
 * @brief Exception thrown when performing an operation that is not allowed.
 */
class InvalidOperationException : public MessageException
{
public:
  explicit InvalidOperationException(const std::string& message);
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EXCEPTIONS_H_
