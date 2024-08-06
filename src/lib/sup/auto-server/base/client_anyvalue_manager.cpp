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

#include <sup/auto-server/client_anyvalue_manager.h>



namespace sup
{
namespace auto_server
{

ClientAnyValueManager::ClientAnyValueManager(IJobInfoIO& job_info_io)
  : m_job_info_io{job_info_io}
  , m_cb_map{}
{}

ClientAnyValueManager::~ClientAnyValueManager() = default;

bool ClientAnyValueManager::AddAnyValues(const NameAnyValueSet& name_value_set)
{
  for (auto& name_value_pair : name_value_set)
  {
    auto& value_name = name_value_pair.first;
    if (m_cb_map.find(value_name) != m_cb_map.end())
    {
      return false;
    }
    auto cb = CreateCallback(value_name);
    cb(m_job_info_io, name_value_pair.second);
    m_cb_map[value_name] = cb;
  }
  return true;
}

bool ClientAnyValueManager::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  auto iter = m_cb_map.find(name);
  if (iter == m_cb_map.end())
  {
    return false;
  }
  iter->second(m_job_info_io, value);
  return true;
}

ClientAnyValueManager::AnyValueCallback CreateCallback(const std::string& value_name)
{
  (void)value_name;
  auto dummy = [](IJobInfoIO& job_info_io, const sup::dto::AnyValue& value) {
    (void)job_info_io;
    (void)value;
    return;
  };
  return dummy;
}

}  // namespace auto_server

}  // namespace sup
