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

#include <sup/auto-server/job_proxy.h>
#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{

JobProxy::JobProxy(const sup::dto::AnyValue& job_info_av)
  : m_job_prefix{}
  , m_full_name{}
  , m_vars{}
  , m_instr_tree_info{}
{
  // TODO: add validation of main structure
  m_job_prefix = job_info_av[kJobPrefixFieldName].As<std::string>();
  m_full_name = job_info_av[kFullNameFieldName].As<std::string>();
  const auto& variable_info = job_info_av[kVariableInfoFieldName];
  // TODO: construct m_vars from variable_info
  m_instr_tree_info = job_info_av[kInstructionTreeInfoFieldName];
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

}  // namespace auto_server

}  // namespace sup
