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

#include "epics_server.h"

#include <sup/epics/pv_access_server.h>

namespace sup
{
namespace auto_server
{

EPICSServer::EPICSServer(const AnyValueServerInterface::NameAnyValueSet& name_value_set)
  : m_update_queue{}
  , m_update_future{}
{
  m_update_future = std::async(std::launch::async, &EPICSServer::UpdateLoop, this, name_value_set);
}

EPICSServer::~EPICSServer()
{
  m_update_queue.PushExit();
}

void EPICSServer::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  m_update_queue.Push(name, value);
}

void EPICSServer::UpdateLoop(const AnyValueServerInterface::NameAnyValueSet& name_value_set)
{
  sup::epics::PvAccessServer server;
  for (const auto& name_value_pair : name_value_set)
  {
    server.AddVariable(name_value_pair.first, name_value_pair.second);
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
