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

#include <sup/auto-server/client_interface_mapper.h>

#include <sup/auto-server/sup_auto_protocol.h>

#include "variable_utils.h"

namespace sup
{
namespace auto_server
{


ClientInterfaceMapper::ClientInterfaceMapper(sequencer::JobInterface& job_interface,
                                             const JobInfo& job_info)
  : m_job_interface{job_interface}
  , m_instr_info{}
  , m_var_name_map{utils::BuildVariableNameMap(job_info.GetVariableInfo())}
{}

ClientInterfaceMapper::~ClientInterfaceMapper() = default;

void ClientInterfaceMapper::InstructionUpdated(sup::dto::uint32 instr_idx,
                                               const sup::dto::AnyValue& value)
{
  (void)instr_idx;
  (void)value;
  // check what changed: status or breakpoint
  // and call the appropriate JobInterface method
}

void ClientInterfaceMapper::VariableUpdated(
  sup::dto::uint32 var_idx, const sup::dto::AnyValue& value)
{
  if (var_idx >= m_var_name_map.size())
  {
    return;
  }
  const auto& var_name = m_var_name_map[var_idx];
  auto var_info = DecodeVariableInfo(value);
  m_job_interface.VariableUpdated(var_name, var_info.first, var_info.second);
}

}  // namespace auto_server

}  // namespace sup
