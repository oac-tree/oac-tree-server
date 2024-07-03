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

#include "epics_anyvalue_manager_registry.h"

#include "epics_anyvalue_manager.h"

namespace sup
{
namespace auto_server
{

EPICSAnyValueManagerRegistry::EPICSAnyValueManagerRegistry(std::size_t n_managers)
  : m_anyvalue_managers{}
{
  m_anyvalue_managers.reserve(n_managers);
  for (std::size_t idx = 0; idx < n_managers; ++idx)
  {
    m_anyvalue_managers.emplace_back(new EPICSAnyValueManager{});
  }
}

EPICSAnyValueManagerRegistry::~EPICSAnyValueManagerRegistry() = default;

IAnyValueManager& EPICSAnyValueManagerRegistry::GetAnyValueManager(std::size_t idx)
{
  auto valid_idx = idx % m_anyvalue_managers.size();
  return *m_anyvalue_managers[valid_idx];
}

}  // namespace auto_server

}  // namespace sup
