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

#ifndef SUP_AUTO_SERVER_SERVER_INTERFACE_H_
#define SUP_AUTO_SERVER_SERVER_INTERFACE_H_

#include <sup/dto/anyvalue.h>

#include <set>
#include <string>
#include <vector>
#include <utility>

namespace sup
{
namespace auto_server
{

/**
 * @brief ServerInterface defines the API for implementations that can handle the different I/O
 * requests for a running procedure. Those interactions in turn are defined by the API of
 * UserInterface and JobStateMonitor.
 *
 * @details Instead of having methods that directly map to all the UserInterface and JobStateMonitor
 * methods, this interface tries to provide more basic functionality, like exposing fixed AnyValues,
 * providing ways to exchange AnyValues, etc.
 *
 * @todo Provide the API for client input and message queues, i.e. values whose history may matter.
 */
class ServerInterface
{
public:
  using NameAnyValuePair = std::pair<std::string, sup::dto::AnyValue>;
  using NameAnyValueSet = std::vector<NameAnyValuePair>;

  virtual ~ServerInterface();

  /**
   * @brief Serve a set of AnyValues with given unique names.
   *
   * @details Typical implementations should be able to support multiple calls to this member
   * function, allowing to serve additional sets of AnyValues in different stages of setting up
   * the full server application. In the case of Sequencer procedures, this is required since
   * the job and variable states need to be server before procedure setup, while the instruction
   * tree AnyValue can only be created afterwards.
   *
   * @param name_value_set List of pairs of names and AnyValues.
   * @return true when successful. In case of failure, none of the values is assumed to be served.
   */
  virtual bool ServeAnyValues(const NameAnyValueSet& name_value_set) = 0;

  /**
   * @brief Update the value of the served AnyValue with the given name.
   *
   * @param name Name of the served AnyValue to be updated.
   * @param value New value for the server AnyValue.
   * @return true on success. Failure may include the case of an unknown name.
   */
  virtual bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) = 0;
};

/**
 * @brief Get a list of all the names in the list of name/value pairs.
 *
 * @param name_value_set List of name/value pairs.
 * @return List of names.
 */
std::set<std::string> GetNames(const ServerInterface::NameAnyValueSet& name_value_set);

/**
 * @brief Verify that all names in the given list are unique among themselves.
 *
 * @param name_value_set List of name/value pairs.
 * @param error_message Error message to throw when validation fails.
 *
 * @throw InvalidOperationException when at least two names are equal.
 */
void ValidateUniqueNames(const ServerInterface::NameAnyValueSet& name_value_set,
                         const std::string& error_message);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SERVER_INTERFACE_H_
