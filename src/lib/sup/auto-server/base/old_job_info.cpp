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

#include <sup/auto-server/old_job_info.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include "variable_utils.h"

namespace sup
{
namespace auto_server
{

OldJobInfo::OldJobInfo(const std::string& prefix, const sup::sequencer::Procedure& proc)
  : m_job_prefix{prefix}
  , m_full_name{sup::sequencer::GetProcedureName(proc)}
  , m_nr_vars{proc.VariableNames().size()}
  , m_nr_instr{}
  , m_variable_info{utils::BuildWorkspaceInfoAnyValue(proc.GetWorkspace())}
  , m_instr_tree_info{}
{}

OldJobInfo::~OldJobInfo() = default;

void OldJobInfo::SetInstructionTreeInfo(const sup::dto::AnyValue& instr_tree_info,
                                     std::size_t nr_instr)
{
  m_instr_tree_info = instr_tree_info;
  m_nr_instr = nr_instr;
}

std::string OldJobInfo::GetPrefix() const
{
  return m_job_prefix;
}

std::string OldJobInfo::GetProcedureName() const
{
  return m_full_name;
}

std::size_t OldJobInfo::GetNumberOfVariables() const
{
  return m_nr_vars;
}

std::size_t OldJobInfo::GetNumberOfInstructions() const
{
  return m_nr_instr;
}

const sup::dto::AnyValue& OldJobInfo::GetWorkspaceInfo() const
{
  return m_variable_info;
}

const sup::dto::AnyValue& OldJobInfo::GetInstructionTreeInfo() const
{
  return m_instr_tree_info;
}

sup::dto::AnyValue ToAnyValue(const OldJobInfo& job_info)
{
  sup::dto::AnyValue result = {{
    { kJobPrefixFieldName, job_info.GetPrefix() },
    { kFullNameFieldName, job_info.GetProcedureName() },
    { kNumberOfVarsFieldName,
      { sup::dto::UnsignedInteger64Type, job_info.GetNumberOfVariables() } },
    { kNumberOfInstrFieldName,
      { sup::dto::UnsignedInteger64Type, job_info.GetNumberOfInstructions() } },
    { kWorkspaceInfoFieldName, job_info.GetWorkspaceInfo() },
    { kInstructionTreeInfoFieldName, job_info.GetInstructionTreeInfo() }
  }};
  return result;
}

}  // namespace auto_server

}  // namespace sup
