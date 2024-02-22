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

#include "epics_pv_handler.h"

#include <sup/auto-server/sup_auto_protocol.h>

namespace
{
using namespace sup::auto_server;
using sup::epics::PvAccessServer;
bool ProcessCommandQueue(std::deque<PVUpdateCommand>& queue, PvAccessServer& server);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{

EPICSPVHandler::EPICSPVHandler(const std::string& prefix, const sup::dto::AnyValue& instr_tree)
  : m_jobstate_channel{GetJobStatePVName(prefix)}
  , m_instruction_tree_channel{GetInstructionTreePVName(prefix)}
  , m_update_queue{}
  , m_update_future{}
  , m_server{}
{
  m_server.AddVariable(m_jobstate_channel, kJobStateAnyValue);
  m_server.AddVariable(m_instruction_tree_channel, instr_tree);
  m_server.Start();
  m_update_future = std::async(std::launch::async, &EPICSPVHandler::UpdateLoop, this);
}

EPICSPVHandler::~EPICSPVHandler()
{
  m_update_queue.PushExit();
}

void EPICSPVHandler::UpdateJobState(const sup::dto::AnyValue& job_state)
{
  m_update_queue.Push(m_jobstate_channel, job_state);
}

void EPICSPVHandler::UpdateInstructionTree(const sup::dto::AnyValue& instr_tree)
{
  m_update_queue.Push(m_instruction_tree_channel, instr_tree);
}

void EPICSPVHandler::UpdateLoop()
{
  bool exit = false;
  while (!exit)
  {
    m_update_queue.WaitForNonEmpty();
    auto queue = m_update_queue.PopCommands();
    exit = ProcessCommandQueue(queue, m_server);
  }
}

}  // namespace auto_server

}  // namespace sup

namespace
{
bool ProcessCommandQueue(std::deque<PVUpdateCommand>& queue, PvAccessServer& server)
{
  bool exit = false;
  while (!queue.empty())
  {
    auto& command = queue.front();
    if (command.GetCommandType() == PVUpdateCommand::kExit)
    {
      exit = true;
      continue;
    }
    server.SetValue(command.Channel(), command.Value());
    queue.pop_front();
  }
  return exit;
}
}  // unnamed namespace
