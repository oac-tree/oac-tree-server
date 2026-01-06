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
 * Copyright (c) : 2010-2026 ITER Organization,
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

#ifndef SUP_OAC_TREE_SERVER_EPICS_ANYVALUE_MANAGER_REGISTRY_H_
#define SUP_OAC_TREE_SERVER_EPICS_ANYVALUE_MANAGER_REGISTRY_H_

#include <sup/oac-tree-server/i_anyvalue_manager_registry.h>

namespace sup
{
namespace oac_tree_server
{

  /**
 * @brief EPICS PvAccess implementation of IAnyValueManagerRegistry. It manages a fixed number of
 * AnyValueManager objects and will return the manager corresponding to the requested index modulo
 * the supported number of managers.
 */
class EPICSAnyValueManagerRegistry : public IAnyValueManagerRegistry
{
public:
  explicit EPICSAnyValueManagerRegistry(sup::dto::uint32 n_managers);
  EPICSAnyValueManagerRegistry(const EPICSAnyValueManagerRegistry &) = delete;
  EPICSAnyValueManagerRegistry(EPICSAnyValueManagerRegistry &&) = delete;
  EPICSAnyValueManagerRegistry &operator=(const EPICSAnyValueManagerRegistry &) = delete;
  EPICSAnyValueManagerRegistry &operator=(EPICSAnyValueManagerRegistry &&) = delete;
  virtual ~EPICSAnyValueManagerRegistry();

  IAnyValueManager& GetAnyValueManager(sup::dto::uint32 idx) override;

private:
  std::vector<std::unique_ptr<IAnyValueManager>> m_anyvalue_managers;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_EPICS_ANYVALUE_MANAGER_REGISTRY_H_
