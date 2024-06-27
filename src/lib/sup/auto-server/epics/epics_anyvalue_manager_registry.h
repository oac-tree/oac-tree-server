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

#ifndef SUP_AUTO_SERVER_EPICS_ANYVALUE_MANAGER_REGISTRY_H_
#define SUP_AUTO_SERVER_EPICS_ANYVALUE_MANAGER_REGISTRY_H_

#include <sup/auto-server/i_anyvalue_manager_registry.h>

namespace sup
{
namespace auto_server
{

  /**
 * @brief EPICS PvAccess implementation of IAnyValueManagerRegistry. It manages a fixed number of
 * AnyValueManager objects and will return the manager corresponding to the requested index modulo
 * the supported number of managers.
 */
class EPICSAnyValueManagerRegistry : public IAnyValueManagerRegistry
{
public:
  EPICSAnyValueManagerRegistry(std::size_t n_managers);
  virtual ~EPICSAnyValueManagerRegistry();

  IAnyValueManager& GetAnyValueManager(std::size_t idx) override;

private:
  std::vector<std::unique_ptr<IAnyValueManager>> m_anyvalue_managers;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_ANYVALUE_MANAGER_REGISTRY_H_
