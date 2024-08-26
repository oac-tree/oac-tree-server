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

#include "epics_anyvalue_manager.h"

#include "epics_server.h"

namespace sup
{
namespace auto_server
{

EPICSAnyValueManager::EPICSAnyValueManager()
  : m_map_mutex{}
  , m_name_server_map{}
  , m_set_servers{}
{}

EPICSAnyValueManager::~EPICSAnyValueManager() = default;

bool EPICSAnyValueManager::AddAnyValues(const NameAnyValueSet &name_value_set)
{
  // Since we are updating the map, we need to hold a lock during the whole operation.
  std::lock_guard<std::mutex> lk{m_map_mutex};
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

bool EPICSAnyValueManager::AddInputServer(const std::string& input_server_name)
{
  (void)input_server_name;
  return true;
}

bool EPICSAnyValueManager::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  // A mutex lock is only needed during the find operation:
  auto server = FindServer(name);
  if (server == nullptr)
  {
    return false;
  }
  server->UpdateAnyValue(name, value);
  return true;
}

sup::dto::AnyValue EPICSAnyValueManager::GetUserInput(const std::string& input_server_name,
                                                      const AnyValueInputRequest& request)
{
  (void)input_server_name;
  (void)request;
  return {};
}

bool EPICSAnyValueManager::ValidateNameValueSet(const NameAnyValueSet& name_value_set) const
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

EPICSServer* EPICSAnyValueManager::FindServer(const std::string& name) const
{
  std::lock_guard<std::mutex> lk{m_map_mutex};
  auto iter = m_name_server_map.find(name);
  if (iter == m_name_server_map.end())
  {
    return nullptr;
  }
  return iter->second;
}

}  // namespace auto_server

}  // namespace sup
