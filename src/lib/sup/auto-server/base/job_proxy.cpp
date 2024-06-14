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
#include "variable_utils.h"

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/job_proxy.h>
#include <sup/auto-server/sup_auto_protocol.h>
#include <sup/auto-server/variable_proxy.h>

#include <set>

namespace
{
using namespace sup::auto_server;

bool ValidateJobInfo(const sup::dto::AnyValue& job_info);
bool ValidateVariableList(const std::vector<VariableProxy>& var_list);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{
const VariableProxy kInvalidVariableProxy( {"", 0, {}} );

JobProxy::JobProxy(const sup::dto::AnyValue& job_info_av)
  : m_job_prefix{}
  , m_full_name{}
  , m_vars{}
  , m_instr_tree_info{}
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
  m_instr_tree_info = job_info_av[kInstructionTreeInfoFieldName];
  // TODO: replace member anyvalue with tree of instr proxies
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
  // TODO: get from instr tree or mapping
  return 0;
}

const std::vector<VariableProxy>& JobProxy::GetWorkspaceInfo() const
{
  return m_vars;
}

const sup::dto::AnyValue& JobProxy::GetInstructionTreeInfo() const
{
  return m_instr_tree_info;
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
  std::vector<VariableProxy> vars(mem_names.size(), kInvalidVariableProxy);
  for (const auto& mem_name : mem_names)
  {
    auto var_info = utils::ToVariableInfo(ws_info_av[mem_name]);
    if (var_info.m_index >= vars.size())
    {
      const std::string error = "JobProxy::InitializeWorkspaceInfo(): encountered index "
                                "larger or equal to number of variables";
      throw InvalidOperationException(error);
    }
    vars[var_info.m_index] = VariableProxy{var_info};
  }
  if (!ValidateVariableList(vars))
  {
    const std::string error = "JobProxy::InitializeWorkspaceInfo(): indices of variables parsed "
                              "do not span exactly [0, n_vars - 1]";
    throw InvalidOperationException(error);
  }
  m_vars = vars;
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

bool ValidateVariableList(const std::vector<VariableProxy>& var_list)
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

}  // unnamed namespace

