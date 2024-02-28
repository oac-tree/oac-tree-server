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

#include "job_pv_info.h"

#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{

JobPVInfo::JobPVInfo(const std::string& prefix, const sup::dto::AnyValue& instr_tree,
            sup::dto::uint32 n_variables)
  : m_prefix{prefix}
  , m_instr_tree{instr_tree}
  , m_n_variables{n_variables}
{}

JobPVInfo::~JobPVInfo() = default;

std::string JobPVInfo::GetJobStateChannel() const
{
  return GetJobStatePVName(m_prefix);
}

std::string JobPVInfo::GetInstructionTreeChannel() const
{
  return GetInstructionTreePVName(m_prefix);
}

sup::dto::uint32 JobPVInfo::GetNumberOfVariables() const
{
  return m_n_variables;
}

std::string JobPVInfo::GetVariableChannel(sup::dto::uint32 index) const
{
  if (index < m_n_variables)
  {
    return GetVariablePVName(m_prefix, index);
  }
  return {};
}

sup::dto::AnyValue JobPVInfo::GetInstructionTreeStructure() const
{
  return m_instr_tree;
}

}  // namespace auto_server

}  // namespace sup
