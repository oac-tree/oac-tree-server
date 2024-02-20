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

#include <sup/auto-server/server_ui.h>

#include <sup/auto-server/base/instruction_tree_utils.h>

#include <sup/sequencer/instruction.h>

namespace sup
{
namespace auto_server
{

ServerUserInterface::ServerUserInterface(const sequencer::Instruction* root_instruction,
                                         IJobPVServer& pv_server)
  : m_instr_tree_cache{root_instruction}
  , m_tree_anyvalue{m_instr_tree_cache.GetInitialInstructionTreeAnyValue()}
  , m_pv_server{pv_server}
{}

ServerUserInterface::~ServerUserInterface() = default;

void ServerUserInterface::UpdateInstructionStatus(const sequencer::Instruction* instruction)
{
  auto path = m_instr_tree_cache.FindInstructionPath(instruction);
  auto status = instruction->GetStatus();
  m_tree_anyvalue[path][utils::kExecStatusField] = static_cast<sup::dto::uint16>(status);
  m_pv_server.UpdateInstructionTreePV(m_tree_anyvalue);
}

void ServerUserInterface::VariableUpdated(const std::string& name, const sup::dto::AnyValue& value,
                                          bool connected)
{
  (void)name;
  (void)value;
  (void)connected;
}

bool ServerUserInterface::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{
  (void)value;
  (void)description;
  return true;
}

bool ServerUserInterface::GetUserValue(sup::dto::AnyValue& value, const std::string& description)
{
  (void)value;
  (void)description;
  return false;
}

int ServerUserInterface::GetUserChoice(const std::vector<std::string>& options,
                                       const sup::dto::AnyValue& metadata)
{
  (void)options;
  (void)metadata;
  return -1;
}

void ServerUserInterface::Message(const std::string& message)
{
  (void)message;
}

void ServerUserInterface::Log(int severity, const std::string& message)
{
  (void)severity;
  (void)message;
}

}  // namespace auto_server

}  // namespace sup
