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

JobPVInfo::JobPVInfo(const std::string& prefix, const sup::dto::AnyValue& instr_tree)
  : m_jobstate_channel{GetJobStatePVName(prefix)}
  , m_instruction_tree_channel{GetInstructionTreePVName(prefix)}
  , m_instr_tree{instr_tree}
{}

JobPVInfo::~JobPVInfo() = default;

std::string JobPVInfo::GetJobStateChannel() const
{
  return m_jobstate_channel;
}

std::string JobPVInfo::GetInstructionTreeChannel() const
{
  return m_instruction_tree_channel;
}

sup::dto::AnyValue JobPVInfo::GetInstructionTreeStructure() const
{
  return m_instr_tree;
}

}  // namespace auto_server

}  // namespace sup
