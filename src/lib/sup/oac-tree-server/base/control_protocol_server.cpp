/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : oac-tree server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/oac-tree-server/control_protocol_server.h>

#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/protocol/function_protocol_extract.h>
#include <sup/protocol/protocol_rpc.h>

namespace
{
sup::protocol::ProtocolResult ExtractJobCommand(const sup::dto::AnyValue& input,
                                                sup::oac_tree::JobCommand& command);
}  // unnamed namespace

namespace sup
{
namespace oac_tree_server
{

ControlProtocolServer::ControlProtocolServer(IJobManager& job_manager)
  : m_job_manager{job_manager}
{}

ControlProtocolServer::~ControlProtocolServer() = default;

sup::protocol::ProtocolResult ControlProtocolServer::Invoke(const sup::dto::AnyValue& input,
                                                            sup::dto::AnyValue& output)
{
  return sup::protocol::CallFunctionProtocol(*this, FunctionMap(), input, output, NotSupported);
}

sup::protocol::ProtocolResult ControlProtocolServer::Service(const sup::dto::AnyValue& input,
                                                             sup::dto::AnyValue& output)
{
  if (sup::protocol::utils::IsApplicationProtocolRequestPayload(input))
  {
    return sup::protocol::utils::HandleApplicationProtocolInfo(
      output, kAutomationControlServerProtocolServerType,
      kAutomationControlServerProtocolServerVersion);
  }
  return NotSupported;
}

const sup::protocol::ProtocolMemberFunctionMap<ControlProtocolServer>&
ControlProtocolServer::FunctionMap()
{
  static sup::protocol::ProtocolMemberFunctionMap<ControlProtocolServer> f_map = {
    { kEditBreakpointCommandFunctionName, &ControlProtocolServer::EditBreakpoint },
    { kSendJobCommandFunctionName, &ControlProtocolServer::SendJobCommand }
  };
  return f_map;
}

sup::protocol::ProtocolResult ControlProtocolServer::EditBreakpoint(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  (void)output;
  sup::dto::uint32 job_idx{};
  auto result = ExtractJobIndex(input, m_job_manager.GetNumberOfJobs(), job_idx);
  if (result != sup::protocol::Success)
  {
    return result;
  }
  sup::dto::uint32 instr_idx{};
  auto number_of_instructions = m_job_manager.GetJobInfo(job_idx).GetNumberOfInstructions();
  result = ExtractInstructionIndex(input, number_of_instructions, instr_idx);
  if (result != sup::protocol::Success)
  {
    return result;
  }
  bool breakpoint_active;
  if (!sup::protocol::FunctionProtocolExtract(breakpoint_active, input, kBreakpointActiveFieldName))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  m_job_manager.EditBreakpoint(job_idx, instr_idx, breakpoint_active);
  return sup::protocol::Success;
}

sup::protocol::ProtocolResult ControlProtocolServer::SendJobCommand(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  (void)output;
  sup::dto::uint32 idx{};
  auto result = ExtractJobIndex(input, m_job_manager.GetNumberOfJobs(), idx);
  if (result != sup::protocol::Success)
  {
    return result;
  }
  sup::oac_tree::JobCommand command{sup::oac_tree::JobCommand::kStart};
  result = ExtractJobCommand(input, command);
  if (result != sup::protocol::Success)
  {
    return result;
  }
  m_job_manager.SendJobCommand(idx, command);
  return sup::protocol::Success;
}

}  // namespace oac_tree_server

}  // namespace sup

namespace
{
using namespace sup::oac_tree_server;
sup::protocol::ProtocolResult ExtractJobCommand(const sup::dto::AnyValue& input,
                                                sup::oac_tree::JobCommand& command)
{
  sup::dto::AnyValue command_av;
  if (!sup::protocol::FunctionProtocolExtract(command_av, input, kJobCommandFieldName))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  sup::dto::uint32 command_int;
  if (!command_av.As(command_int))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  if (command_int >= static_cast<sup::dto::uint32>(sup::oac_tree::JobCommand::kTerminate))
  {
    return UnknownJobCommand;
  }
  command = static_cast<sup::oac_tree::JobCommand>(command_int);
  return sup::protocol::Success;
}

}  // unnamed namespace

