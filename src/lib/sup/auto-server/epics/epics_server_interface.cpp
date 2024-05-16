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

#include "epics_server_interface.h"

namespace sup
{
namespace auto_server
{

EPICSServerInterface::EPICSServerInterface()
  : m_name_server_map{}
  , m_set_servers{}
{}

EPICSServerInterface::~EPICSServerInterface() = default;

bool EPICSServerInterface::ServeAnyValues(const NameAnyValueSet &name_value_set)
{
  if (!ValidateNameValueSet(name_value_set))
  {
    return false;
  }
  auto names = GetNames(name_value_set);
  std::unique_ptr<EPICSServer> server{new EPICSServer(name_value_set)};
  for (const auto &name : names)
  {
    m_name_server_map[name] = server.get();
  }
  m_set_servers.emplace_back(std::move(server));
  return true;
}

// TODO: ensure thread safety. These calls could come concurrently and the map iterators may
// become invalidated.
bool EPICSServerInterface::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  auto iter = m_name_server_map.find(name);
  if (iter == m_name_server_map.end())
  {
    return false;
  }
  iter->second->UpdateAnyValue(name, value);
  return true;
}

bool EPICSServerInterface::ValidateNameValueSet(const NameAnyValueSet& name_value_set) const
{
  auto names = GetNames(name_value_set);
  if (names.size() != name_value_set.size())
  {
    return false;
  }
  for (const auto& name : names)
  {
    auto iter = m_name_server_map.find(name);
    if (iter != m_name_server_map.end())
    {
      return false;
    }
  }
  return true;
}

}  // namespace auto_server

}  // namespace sup
