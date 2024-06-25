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

#ifndef SUP_AUTO_SERVER_ANYVALUE_MANAGER_REGISTRY_H_
#define SUP_AUTO_SERVER_ANYVALUE_MANAGER_REGISTRY_H_

#include <sup/auto-server/anyvalue_manager_interface.h>

namespace sup
{
namespace auto_server
{

  /**
 * @brief Interface for classes that provide AnyValueManagerInterface implementations and manage
 * their lifecycle.
 */
class AnyValueManagerRegistry
{
public:
  virtual ~AnyValueManagerRegistry();

  /**
   * @brief Get a reference to an AnyValueManager object.
   *
   * @details An index can be provided to allow for managing multiple objects. Note however that
   * an implementation of this interface may decide to return the same object for different indices.
   * In the extreme case, the provider may only manage a single AnyValueManager object and always
   * return this, regardless of the index.
   *
   * @param idx Index that identifies which object to return.
   * @return Reference to an AnyValueManagerInterface implementation.
   */
  virtual AnyValueManagerInterface& GetAnyValueManager(std::size_t idx) = 0;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_ANYVALUE_MANAGER_REGISTRY_H_
