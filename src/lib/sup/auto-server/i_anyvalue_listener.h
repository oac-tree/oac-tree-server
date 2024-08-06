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
 * are expected to monitor published AnyValues from an IAnyValueManager and propagates updates to
 * a dedicated member class. This interface is empty but is used to be able to store polymorphic
 * objects of this type.
 *
 * @details The member class that handles updates is typically an IAnyValueManager implementation.
 */
class IAnyValueListener
{
public:
  virtual ~IAnyValueListener();
};

/**
 * @brief ListenerFactoryFunction defines the signature of a factory function that can be injected
 * into other classes and that will be used to create an IAnyValueListener from a given JobInfo
 * that will forward all its updates to an IAnyValueManager object.
 *
 */
using ListenerFactoryFunction =
  std::function<std::unique_ptr<IAnyValueListener>(const JobInfo&, IAnyValueManager&)>;

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_ANYVALUE_LISTENER_H_
