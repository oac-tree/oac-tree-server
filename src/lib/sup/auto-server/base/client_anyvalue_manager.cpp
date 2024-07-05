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

ClientAnyValueManager::ClientAnyValueManager(std::size_t job_idx,
                                             const JobInfo& job_info,
                                             AutomationClientInterface& client_if)
  : m_job_idx{job_idx}
  , m_value_mapper{job_info}
  , m_client_if{client_if}
  , m_cb_map{}
{}

ClientAnyValueManager::~ClientAnyValueManager() = default;

bool ClientAnyValueManager::AddAnyValues(const NameAnyValueSet& name_value_set)
{
  (void)name_value_set;
  // TODO: initialize mapping between the name and a std::function<void(const AnyValue&)> that
  // will propagate the parsed value to m_client_if
  return true;
}

bool ClientAnyValueManager::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  auto iter = m_cb_map.find(name);
  if (iter == m_cb_map.end())
  {
    return false;
  }
  iter->second(value);
  return true;
}

ClientAnyValueManager::AnyValueCallback ClientAnyValueManager::GetInstructionUpdateFunction(
  const std::string& val_name)
{
  auto instr_info = m_value_mapper.FindInstructionInfo(val_name);
  auto func = [this, instr_info](const sup::dto::AnyValue& val) {
    auto instr_state = ToInstructionState(val);
    m_client_if.InstructionUpdated(m_job_idx, instr_info, instr_state);
  };
  return func;
}

}  // namespace auto_server

}  // namespace sup
