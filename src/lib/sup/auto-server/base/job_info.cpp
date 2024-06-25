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

namespace sup
{
namespace auto_server
{
JobInfo::JobInfo(const std::string& job_prefix, const std::string& full_name,
                   const WorkspaceInfo& ws_info, std::unique_ptr<InstructionInfo> root_info)
  : m_job_prefix{job_prefix}
  , m_full_name{full_name}
  , m_ws{ws_info}
  , m_root{std::move(root_info)}
  , m_ordered_instr{utils::CreateOrderedInstructionInfo(*m_root)}
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
  return m_ordered_instr.size();
}

const WorkspaceInfo& JobInfo::GetWorkspaceInfo() const
{
  return m_ws;
}

const InstructionInfo* JobInfo::GetRootInstructionInfo() const
{
  return m_root.get();
}

bool operator==(const JobInfo& left, const JobInfo& right)
{
  if (left.GetPrefix() != right.GetPrefix())
  {
    return false;
  }
  if (left.GetProcedureName() != right.GetProcedureName())
  {
    return false;
  }
  if (left.GetWorkspaceInfo() != right.GetWorkspaceInfo())
  {
    return false;
  }
  if (*left.GetRootInstructionInfo() != *right.GetRootInstructionInfo())
  {
    return false;
  }
  return true;
}

bool operator!=(const JobInfo& left, const JobInfo& right)
{
  return !(left == right);
}

}  // namespace auto_server

}  // namespace sup
