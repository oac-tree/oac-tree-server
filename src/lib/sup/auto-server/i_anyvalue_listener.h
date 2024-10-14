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

#ifndef SUP_AUTO_SERVER_I_ANYVALUE_LISTENER_H_
#define SUP_AUTO_SERVER_I_ANYVALUE_LISTENER_H_

#include <sup/auto-server/job_info.h>
#include <sup/auto-server/i_anyvalue_manager.h>

#include <functional>
#include <memory>


namespace sup
{
namespace auto_server
{

/**
 * @brief This is the interface definition of the counterpart of IAnyValueManager. Implementations
 * are expected to monitor published AnyValues from an IAnyValueManager and propagate updates to
 * a dedicated member class.
 *
 * @details The member class that handles updates is typically another IAnyValueManager
 * implementation.
 */
class IAnyValueListener
{
public:
  virtual ~IAnyValueListener();

  /**
   * @brief Add a set of AnyValues with given unique names that need to be monitored.
   *
   * @param monitor_set List of pairs of names and AnyValues to monitor.
   * @return true when successful. In case of failure, none of the values is assumed to be added.
   */
  virtual bool AddAnyValues(const IAnyValueManager::NameAnyValueSet& monitor_set) = 0;

  /**
   * @brief Add a client that can handle requests for user input.
   *
   * @param input_server_name Name of the corresponding input server.
   * @return true when successful. In case of failure, no client is instantiated.
   */
  virtual bool AddInputHandler(const std::string& input_server_name) = 0;
};

/**
 * @brief ListenerFactoryFunction defines the signature of a factory function that can be injected
 * into other classes and that will be used to create an IAnyValueListener that will forward all its
 * updates to an IAnyValueManager object.
 */
using ListenerFactoryFunction =
  std::function<std::unique_ptr<IAnyValueListener>(IAnyValueManager&)>;

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_ANYVALUE_LISTENER_H_
