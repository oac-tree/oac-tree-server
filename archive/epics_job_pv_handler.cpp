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

#include "epics_job_pv_handler.h"

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/epics/pv_access_server.h>

namespace sup
{
namespace auto_server
{

EPICSJobPVHandler::EPICSJobPVHandler(const std::string& prefix, sup::dto::uint32 n_variables)
  : m_update_queue{}
  , m_prefix{prefix}
  , m_update_future{}
{
  m_update_future = std::async(std::launch::async, &EPICSJobPVHandler::UpdateLoop, this, n_variables);
}

EPICSJobPVHandler::~EPICSJobPVHandler()
{
  m_update_queue.PushExit();
}

void EPICSJobPVHandler::UpdateJobState(const sup::dto::AnyValue& job_state)
{
  m_update_queue.Push(GetJobStatePVName(m_prefix), job_state);
}

void EPICSJobPVHandler::UpdateVariable(sup::dto::uint32 index, const sup::dto::AnyValue& var_info)
{
  m_update_queue.Push(GetVariablePVName(m_prefix, index), var_info);
}

void EPICSJobPVHandler::UpdateLoop(sup::dto::uint32 n_variables)
{
  sup::epics::PvAccessServer server;
  server.AddVariable(GetJobStatePVName(m_prefix), kJobStateAnyValue);
  for (sup::dto::uint32 idx = 0; idx < n_variables; ++idx)
  {
    server.AddVariable(GetVariablePVName(m_prefix, idx), kVariableAnyValue);
  }
  server.Start();
  bool exit = false;
  auto update_func = [&server](const std::string& channel, const sup::dto::AnyValue& value) {
    server.SetValue(channel, value);
  };
  while (!exit)
  {
    m_update_queue.WaitForNonEmpty();
    auto queue = m_update_queue.PopCommands();
    exit = ProcessCommandQueue(queue, update_func);
  }
}

}  // namespace auto_server

}  // namespace sup
