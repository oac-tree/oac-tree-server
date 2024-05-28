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
  return m_nr_vars;
}


}  // namespace auto_server

}  // namespace sup
