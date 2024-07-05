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

#include <sup/auto-server/job_info_value_mapper.h>

#include <sup/auto-server/sup_auto_protocol.h>

#include <algorithm>
#include <deque>

namespace sup
{
namespace auto_server
{

JobInfoValueMapper::JobInfoValueMapper(const JobInfo& job_info)
  : m_job_state_val_name{GetJobStatePVName(job_info.GetPrefix())}
  , m_instr_map{}
  , m_var_map{}
{
  InitInstructionMap(job_info);
  InitVariableMap(job_info);
}

JobInfoValueMapper::~JobInfoValueMapper() = default;

std::string JobInfoValueMapper::GetJobStateValueName() const
{
  return m_job_state_val_name;
}

std::vector<std::string> JobInfoValueMapper::GetInstructionValueNames() const
{
  std::vector<std::string> result;
  auto op = [](const decltype(m_instr_map)::value_type& entry) {
    return entry.first;
  };
  std::transform(m_instr_map.begin(), m_instr_map.end(), std::back_inserter(result), op);
  return result;
}

const InstructionInfo* JobInfoValueMapper::FindInstructionInfo(const std::string& val_name) const
{
  auto iter = m_instr_map.find(val_name);
  if (iter == m_instr_map.end())
  {
    return nullptr;
  }
  return iter->second;
}

std::vector<std::string> JobInfoValueMapper::GetVariableValueNames() const
{
  std::vector<std::string> result;
  auto op = [](const decltype(m_var_map)::value_type& entry) {
    return entry.first;
  };
  std::transform(m_var_map.begin(), m_var_map.end(), std::back_inserter(result), op);
  return result;
}

std::string JobInfoValueMapper::FindVariableName(const std::string& val_name) const
{
  auto iter = m_var_map.find(val_name);
  if (iter == m_var_map.end())
  {
    return "";
  }
  return iter->second;
}

void JobInfoValueMapper::InitInstructionMap(const JobInfo& job_info)
{
  auto prefix = job_info.GetPrefix();
  const auto* root_instr = job_info.GetRootInstructionInfo();
  std::deque<const InstructionInfo*> stack;
  stack.push_back(root_instr);
  while (!stack.empty())
  {
    auto node = stack.back();
    stack.pop_back();
    auto instr_idx = node->GetIndex();
    auto instr_val_name = GetInstructionPVName(prefix, instr_idx);
    m_instr_map[instr_val_name] = node;
    for (const auto* child_instr : node->Children())
    {
      stack.push_back(child_instr);
    }
  }
}

void JobInfoValueMapper::InitVariableMap(const JobInfo& job_info)
{
  auto prefix = job_info.GetPrefix();
  const auto& ws_info = job_info.GetWorkspaceInfo();
  const auto& var_infos = ws_info.GetVariableInfos();
  for (const auto& var_info : var_infos)
  {
    auto var_name = var_info.first;
    auto var_idx = var_info.second.GetIndex();
    auto val_name = GetVariablePVName(prefix, var_idx);
    m_var_map[val_name] = var_name;
  }
}

}  // namespace auto_server

}  // namespace sup
