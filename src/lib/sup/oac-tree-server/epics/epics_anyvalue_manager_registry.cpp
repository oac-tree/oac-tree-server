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

#include "epics_anyvalue_manager_registry.h"

#include "epics_anyvalue_manager.h"

namespace sup
{
namespace oac_tree_server
{

EPICSAnyValueManagerRegistry::EPICSAnyValueManagerRegistry(sup::dto::uint32 n_managers)
  : m_anyvalue_managers{}
{
  m_anyvalue_managers.reserve(n_managers);
  for (sup::dto::uint32 idx = 0; idx < n_managers; ++idx)
  {
    (void)m_anyvalue_managers.emplace_back(std::make_unique<EPICSAnyValueManager>());
  }
}

EPICSAnyValueManagerRegistry::~EPICSAnyValueManagerRegistry() = default;

IAnyValueManager& EPICSAnyValueManagerRegistry::GetAnyValueManager(sup::dto::uint32 idx)
{
  auto valid_idx = idx % m_anyvalue_managers.size();
  return *m_anyvalue_managers[valid_idx];
}

}  // namespace oac_tree_server

}  // namespace sup
