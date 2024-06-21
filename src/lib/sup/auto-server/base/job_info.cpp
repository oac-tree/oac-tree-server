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
#include <sup/auto-server/job_info.h>
#include <sup/auto-server/sup_auto_protocol.h>
#include <sup/auto-server/variable_info.h>

#include <deque>
#include <set>

namespace
{
using namespace sup::auto_server;

bool ValidateJobInfo(const sup::dto::AnyValue& job_info);
bool ValidateVariableList(const std::vector<VariableInfo>& var_list);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{
const VariableInfo kInvalidVariableProxy{"", 0, {}};

JobInfo::JobInfo(const std::string& job_prefix, const std::string& full_name,
                   const WorkspaceInfo& ws_info, std::unique_ptr<InstructionInfo> root_info)
  : m_job_prefix{job_prefix}
  , m_full_name{full_name}
  , m_ws{ws_info}
  , m_root{std::move(root_info)}
{}

JobInfo::~JobInfo() = default;

std::string JobInfo::GetPrefix() const
{
  return m_job_prefix;
}

std::string JobInfo::GetProcedureName() const
{
  return m_full_name;
}

std::size_t JobInfo::GetNumberOfVariables() const
{
  return m_ws.GetNumberOfVariables();
}

std::size_t JobInfo::GetNumberOfInstructions() const
{
  // TODO: Fix this!
  return 0;
}

const WorkspaceInfo& JobInfo::GetWorkspaceInfo() const
{
  return m_ws;
}

const InstructionInfo* JobInfo::GetRootInstructionInfo() const
{
  return m_root.get();
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

}  // unnamed namespace

