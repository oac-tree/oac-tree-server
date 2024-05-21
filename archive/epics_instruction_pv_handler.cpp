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

#include "epics_instruction_pv_handler.h"

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/epics/pv_access_server.h>

namespace sup
{
namespace auto_server
{

EPICSInstructionPVHandler::EPICSInstructionPVHandler(const std::string& prefix,
                                                     const sup::dto::AnyValue& instr_tree)
  : m_update_queue{}
  , m_prefix{prefix}
  , m_update_future{}
{
  m_update_future =
    std::async(std::launch::async, &EPICSInstructionPVHandler::UpdateLoop, this, instr_tree);
}

EPICSInstructionPVHandler::~EPICSInstructionPVHandler()
{
  m_update_queue.PushExit();
}

void EPICSInstructionPVHandler::UpdateInstructionTree(const sup::dto::AnyValue& instr_tree)
{
  m_update_queue.Push(GetInstructionTreePVName(m_prefix), instr_tree);
}

void EPICSInstructionPVHandler::UpdateLoop(const sup::dto::AnyValue& instr_tree)
{
  sup::epics::PvAccessServer server;
  server.AddVariable(GetInstructionTreePVName(m_prefix), instr_tree);
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
