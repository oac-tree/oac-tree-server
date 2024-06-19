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

#include "anyvalue_utils.h"
#include "instruction_tree_utils.h"
#include "variable_utils.h"

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/job_proxy.h>
#include <sup/auto-server/sup_auto_protocol.h>
#include <sup/auto-server/variable_info.h>

#include <deque>
#include <set>

namespace
{
using namespace sup::auto_server;

bool ValidateJobInfo(const sup::dto::AnyValue& job_info);
bool ValidateVariableList(const std::vector<VariableInfo>& var_list);

struct InstrProxyNode
{
  const sup::dto::AnyValue& node_av;
  InstructionInfo* instr_proxy;
};

std::unique_ptr<InstructionInfo> CreateAndAdd(const sup::dto::AnyValue& instr_av,
                                               std::vector<InstructionInfo*>& instr_map,
                                               std::set<std::size_t>& used_indices);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{
const VariableInfo kInvalidVariableProxy{"", 0, {}};

JobProxy::JobProxy(const sup::dto::AnyValue& job_info_av)
  : m_job_prefix{}
  , m_full_name{}
  , m_vars{}
  , m_root{}
  , m_instr_map{}
{
  if (!ValidateJobInfo(job_info_av))
  {
    const std::string error = "JobProxy ctor: wrong format of job info AnyValue";
    throw InvalidOperationException(error);
  }
  m_job_prefix = job_info_av[kJobPrefixFieldName].As<std::string>();
  m_full_name = job_info_av[kFullNameFieldName].As<std::string>();
  const auto& workspace_info = job_info_av[kVariableInfoFieldName];
  InitializeWorkspaceInfo(workspace_info);
  const auto& instr_tree_info = job_info_av[kInstructionTreeInfoFieldName];
  std::size_t n_instr = job_info_av[kNumberOfInstrFieldName].As<sup::dto::uint64>();
  InitializeInstructionInfo(instr_tree_info, n_instr);
}

JobProxy::~JobProxy() = default;

std::string JobProxy::GetPrefix() const
{
  return m_job_prefix;
}

std::string JobProxy::GetProcedureName() const
{
  return m_full_name;
}

std::size_t JobProxy::GetNumberOfVariables() const
{
  return m_vars.size();
}

std::size_t JobProxy::GetNumberOfInstructions() const
{
  return m_instr_map.size();
}

const std::vector<VariableInfo>& JobProxy::GetWorkspaceInfo() const
{
  return m_vars;
}

const InstructionInfo* JobProxy::GetRootInstructionInfo() const
{
  return m_root.get();
}

void JobProxy::InitializeWorkspaceInfo(const sup::dto::AnyValue& ws_info_av)
{
  if (!sup::dto::IsStructValue(ws_info_av))
  {
    const std::string error = "JobProxy::InitializeWorkspaceInfo(): wrong format of "
                              "workspace info AnyValue";
    throw InvalidOperationException(error);
  }
  auto mem_names = ws_info_av.MemberNames();
  std::vector<VariableInfo> vars(mem_names.size(), kInvalidVariableProxy);
  for (const auto& mem_name : mem_names)
  {
    auto var_info = utils::ToVariableInfo(ws_info_av[mem_name]);
    if (var_info.GetIndex() >= vars.size())
    {
      const std::string error = "JobProxy::InitializeWorkspaceInfo(): encountered index "
                                "larger or equal to number of variables";
      throw InvalidOperationException(error);
    }
    vars[var_info.GetIndex()] = var_info;
  }
  if (!ValidateVariableList(vars))
  {
    const std::string error = "JobProxy::InitializeWorkspaceInfo(): indices of variables parsed "
                              "do not span exactly [0, n_vars - 1]";
    throw InvalidOperationException(error);
  }
  m_vars = vars;
}

void JobProxy::InitializeInstructionInfo(const sup::dto::AnyValue& instr_info_av,
                                         std::size_t n_instr)
{
  if (!sup::dto::IsStructValue(instr_info_av))
  {
    const std::string error = "JobProxy::InitializeInstructionInfo(): wrong format of "
                              "instruction tree info AnyValue";
    throw InvalidOperationException(error);
  }
  std::vector<InstructionInfo*> instr_map(n_instr, nullptr);
  std::set<std::size_t> used_indices{};
  std::deque<InstrProxyNode> stack{};
  auto root = CreateAndAdd(instr_info_av, instr_map, used_indices);
  stack.push_back(InstrProxyNode{instr_info_av, root.get()});
  while (!stack.empty())
  {
    auto current = stack.back();
    stack.pop_back();
    if (current.node_av.HasField(kChildInstructionsField))
    {
      auto& children_av = current.node_av[kChildInstructionsField];
      std::size_t child_idx = 0;
      auto child_mem_name = utils::CreateIndexedMemberName(child_idx);
      while (children_av.HasField(child_mem_name))
      {
        auto& child_av = children_av[child_mem_name];
        auto child = CreateAndAdd(child_av, instr_map, used_indices);
        auto p_child = current.instr_proxy->AppendChild(std::move(child));
        stack.push_back(InstrProxyNode{child_av, p_child});
        ++child_idx;
        child_mem_name = utils::CreateIndexedMemberName(child_idx);
      }
    }
  }
  if (used_indices.size() != n_instr)
  {
    const std::string error = "JobProxy::InitializeInstructionInfo(): duplicate instruction "
                              "indices found";
    throw InvalidOperationException(error);
  }
  m_root = std::move(root);
  m_instr_map = instr_map;
}

std::vector<InstructionInfo*> JobProxy::GetInstructionIndexMap() const
{
  return m_instr_map;
}

}  // namespace auto_server

}  // namespace sup

namespace
{
bool ValidateJobInfo(const sup::dto::AnyValue& job_info)
{
  if (!utils::ValidateMemberType(job_info, kJobPrefixFieldName, sup::dto::StringType))
  {
    return false;
  }
  if (!utils::ValidateMemberType(job_info, kFullNameFieldName, sup::dto::StringType))
  {
    return false;
  }
  if (!utils::ValidateMemberType(job_info, kNumberOfVarsFieldName, sup::dto::UnsignedInteger64Type))
  {
    return false;
  }
  if (!utils::ValidateMemberType(job_info, kNumberOfInstrFieldName, sup::dto::UnsignedInteger64Type))
  {
    return false;
  }
  if (!job_info.HasField(kVariableInfoFieldName))
  {
    return false;
  }
  if (!job_info.HasField(kInstructionTreeInfoFieldName))
  {
    return false;
  }
  return true;
}

bool ValidateVariableList(const std::vector<VariableInfo>& var_list)
{
  std::set<sup::dto::uint32> indices;
  for (const auto& var : var_list)
  {
    if (var.GetType().empty())
    {
      return false;
    }
    indices.insert(var.GetIndex());
  }
  if (indices.size() != var_list.size())
  {
    return false;
  }
  return true;
}

std::unique_ptr<InstructionInfo> CreateAndAdd(const sup::dto::AnyValue& instr_av,
                                               std::vector<InstructionInfo*>& instr_map,
                                               std::set<std::size_t>& used_indices)
{
  auto result = utils::ToInstructionInfo(instr_av);
  auto idx = result->GetIndex();
  if (idx >= instr_map.size())
  {
    const std::string error = "JobProxy::InitializeInstructionInfo(): encountered instruction "
                              "index greater than or equal to number of instructions";
    throw InvalidOperationException(error);
  }
  instr_map[idx] = result.get();
  used_indices.insert(idx);
  return result;
}

}  // unnamed namespace

