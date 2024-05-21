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

#include <sup/auto-server/server_job_interface.h>

#include <sup/auto-server/base/instruction_tree_utils.h>

#include <sup/sequencer/instruction.h>

namespace sup
{
namespace auto_server
{

ServerJobInterface::ServerJobInterface(IJobPVServer& pv_server)
  : m_pv_server{pv_server}
{}

ServerJobInterface::~ServerJobInterface() = default;

void ServerJobInterface::UpdateInstructionStatus(const sequencer::Instruction* instruction)
{
  auto status = instruction->GetStatus();
  m_pv_server.UpdateInstructionStatusPV(instruction, status);
}

void ServerJobInterface::VariableUpdated(const std::string& name, const sup::dto::AnyValue& value,
                                          bool connected)
{
  m_pv_server.UpdateVariable(name, value, connected);
}

bool ServerJobInterface::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{
  (void)value;
  (void)description;
  return true;
}

bool ServerJobInterface::GetUserValue(sup::dto::AnyValue& value, const std::string& description)
{
  (void)value;
  (void)description;
  return false;
}

int ServerJobInterface::GetUserChoice(const std::vector<std::string>& options,
                                       const sup::dto::AnyValue& metadata)
{
  (void)options;
  (void)metadata;
  return -1;
}

void ServerJobInterface::Message(const std::string& message)
{
  (void)message;
}

void ServerJobInterface::Log(int severity, const std::string& message)
{
  (void)severity;
  (void)message;
}

void ServerJobInterface::OnStateChange(sequencer::JobState state) noexcept
{
  m_pv_server.UpdateJobStatePV(state);
}

void ServerJobInterface::OnBreakpointChange(const sequencer::Instruction* instruction,
                                            bool breakpoint_set) noexcept
{
  m_pv_server.UpdateInstructionBreakpointPV(instruction, breakpoint_set);
}

void ServerJobInterface::OnProcedureTick(const sequencer::Procedure& proc) noexcept
{
  // TODO: update next instruction leaves
  (void)proc;
}

}  // namespace auto_server

}  // namespace sup
