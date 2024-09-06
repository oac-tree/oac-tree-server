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

#include "epics_input_server.h"
#include "epics_server.h"

#include <sup/auto-server/anyvalue_input_request.h>
#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{

EPICSAnyValueManager::EPICSAnyValueManager()
  : m_map_mtx{}
  , m_user_input_mtx{}
  , m_name_server_map{}
  , m_servers{}
  , m_name_input_server_map{}
  , m_input_servers{}
{}

EPICSAnyValueManager::~EPICSAnyValueManager() = default;

bool EPICSAnyValueManager::AddAnyValues(const NameAnyValueSet &name_value_set)
{
  // Since we are updating the map, we need to hold a lock during the whole operation.
  std::lock_guard<std::mutex> lk{m_map_mtx};
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
  m_servers.emplace_back(std::move(server));
  return true;
}

bool EPICSAnyValueManager::AddInputServer(const std::string& input_server_name)
{
  {
    std::lock_guard<std::mutex> lk{m_map_mtx};
    auto iter = m_name_input_server_map.find(input_server_name);
    if (iter != m_name_input_server_map.end())
    {
      return false;
    }
    std::unique_ptr<EPICSInputServer> input_server{new EPICSInputServer(input_server_name)};
    m_name_input_server_map[input_server_name] = input_server.get();
    m_input_servers.emplace_back(std::move(input_server));
  }
  auto input_request_name = GetInputRequestPVName(input_server_name);
  NameAnyValueSet value_set;
  value_set.emplace_back(input_request_name, kInputRequestAnyValue);
  if (!AddAnyValues(value_set))
  {
    return false;
  }
  return true;
}

bool EPICSAnyValueManager::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  // The map mutex lock is only needed during the find operation:
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
  std::lock_guard<std::mutex> lk{m_user_input_mtx};
  // The map mutex lock is only needed during the find operation:
  auto input_server = FindInputServer(input_server_name);
  if (input_server == nullptr)
  {
    return {};  // TODO: is this the right return value??
  }
  auto input_request_idx = input_server->InitNewRequest();
  auto input_request = EncodeInputRequest(input_request_idx, request);
  auto input_request_name = GetInputRequestPVName(input_server_name);
  UpdateAnyValue(input_request_name, input_request);
  // TODO: do this in a loop with a small timeout (to allow halting this):
  auto result = input_server->WaitForReply(input_request_idx, 1.0);
  // auto result = input_server->WaitForReply(input_request_idx, 1e9);
  // TODO: what on timeout??
  UpdateAnyValue(input_request_name, kInputRequestAnyValue);
  return result.second;
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
  std::lock_guard<std::mutex> lk{m_map_mtx};
  auto iter = m_name_server_map.find(name);
  if (iter == m_name_server_map.end())
  {
    return nullptr;
  }
  return iter->second;
}

EPICSInputServer* EPICSAnyValueManager::FindInputServer(const std::string& server_name) const
{
  std::lock_guard<std::mutex> lk{m_map_mtx};
  auto iter = m_name_input_server_map.find(server_name);
  if (iter == m_name_input_server_map.end())
  {
    return nullptr;
  }
  return iter->second;
}

}  // namespace auto_server

}  // namespace sup
