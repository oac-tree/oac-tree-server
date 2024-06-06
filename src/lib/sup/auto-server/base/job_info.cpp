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

#include <sup/auto-server/job_info.h>

namespace sup
{
namespace auto_server
{

JobInfo::JobInfo(const std::string& prefix, const sup::sequencer::Procedure& proc)
  : m_job_prefix{prefix}
  , m_full_name{sup::sequencer::GetProcedureName(proc)}
  , m_nr_vars{proc.VariableNames().size()}
  , m_nr_instr{}
  , m_instr_tree_info{}
{}

JobInfo::~JobInfo() = default;

void JobInfo::SetInstructionTreeInfo(const sup::dto::AnyValue& instr_tree_info,
                                     std::size_t nr_instr)
{
  m_instr_tree_info = instr_tree_info;
  m_nr_instr = nr_instr;
}

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
  return m_nr_vars;
}

std::size_t JobInfo::GetNumberOfInstructions() const
{
  return m_nr_instr;
}

const sup::dto::AnyValue& JobInfo::GetInstructionTreeInfo() const
{
  return m_instr_tree_info;
}

}  // namespace auto_server

}  // namespace sup
