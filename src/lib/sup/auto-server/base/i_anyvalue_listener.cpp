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

#include <sup/auto-server/i_anyvalue_listener.h>

#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{

IAnyValueListener::~IAnyValueListener() = default;

IAnyValueManager::NameAnyValueSet GetJobMonitorSet(const JobInfo& job_info)
{
  auto job_prefix = job_info.GetPrefix();
  auto n_vars = job_info.GetNumberOfVariables();
  auto monitor_set = GetInitialValueSet(job_prefix, n_vars);
  auto n_instr = job_info.GetNumberOfInstructions();
  auto instr_set = GetInstructionValueSet(job_prefix, n_instr);
  for (const auto& instr_pair : instr_set)
  {
    monitor_set.push_back(instr_pair);
  }
  return monitor_set;
}

bool ListenToJob(IAnyValueListener& listener, const JobInfo& job_info)
{
  auto monitor_set = GetJobMonitorSet(job_info);
  if (!listener.AddAnyValueMonitors(monitor_set))
  {
    return false;
  }
  auto input_server_name = GetInputServerName(job_info.GetPrefix());
  return listener.AddInputClient(input_server_name);
}

}  // namespace auto_server

}  // namespace sup
