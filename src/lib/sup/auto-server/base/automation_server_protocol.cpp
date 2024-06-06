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

#include <sup/auto-server/automation_server_protocol.h>

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/protocol/function_protocol_extract.h>
#include <sup/protocol/function_protocol_pack.h>
#include <sup/protocol/protocol_rpc.h>

namespace
{
sup::protocol::ProtocolResult ExtractJobCommand(const sup::dto::AnyValue& input,
                                                sup::sequencer::JobCommand& command);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{

namespace status
{
enum AutomationServerStatus
{
  kNotSupported = sup::protocol::SPECIFIC_APPLICATION_ERROR_START,
  kUnknownJob,
  kUnknownInstruction,
  kUnknownJobCommand
};
}  // namespace status

AutomationServerProtocol::AutomationServerProtocol(const std::string& server_prefix,
                         AutomationServer::ProcedureList& proc_list)
  : m_auto_server{server_prefix, proc_list}
{}

AutomationServerProtocol::~AutomationServerProtocol() = default;

sup::protocol::ProtocolResult AutomationServerProtocol::Invoke(const sup::dto::AnyValue& input,
                                                               sup::dto::AnyValue& output)
{
  return sup::protocol::CallFunctionProtocol(*this, FunctionMap(), input, output, NotSupported);
}

sup::protocol::ProtocolResult AutomationServerProtocol::Service(const sup::dto::AnyValue& input,
                                                                sup::dto::AnyValue& output)
{
  if (sup::protocol::utils::IsApplicationProtocolRequestPayload(input))
  {
    return sup::protocol::utils::HandleApplicationProtocolInfo(
      output, kAutomationServerProtocolServerType, kAutomationServerProtocolServerVersion);
  }
  return NotSupported;
}

const sup::protocol::ProtocolMemberFunctionMap<AutomationServerProtocol>&
AutomationServerProtocol::FunctionMap()
{
  static sup::protocol::ProtocolMemberFunctionMap<AutomationServerProtocol> f_map = {
    { kGetServerPrefixFunctionName, &AutomationServerProtocol::GetServerPrefix },
    { kGetNumberOfJobsFunctionName, &AutomationServerProtocol::GetNumberOfJobs },
    { kGetJobInfoFunctionName, &AutomationServerProtocol::GetJobInfo },
    { kEditBreakpointCommandFunctionName, &AutomationServerProtocol::EditBreakpoint },
    { kSendJobCommandFunctionName, &AutomationServerProtocol::SendJobCommand }
  };
  return f_map;
}

sup::protocol::ProtocolResult AutomationServerProtocol::GetServerPrefix(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  (void)input;
  auto server_prefix = m_auto_server.GetServerPrefix();
  sup::dto::AnyValue temp_out;
  sup::protocol::FunctionProtocolPack(temp_out, kServerPrefixFieldName, server_prefix);
  if (!sup::dto::TryAssignIfEmptyOrConvert(output, temp_out))
  {
    return sup::protocol::ServerProtocolEncodingError;
  }
  return sup::protocol::Success;
}

sup::protocol::ProtocolResult AutomationServerProtocol::GetNumberOfJobs(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  (void)input;
  sup::dto::AnyValue number_jobs{sup::dto::UnsignedInteger64Type, m_auto_server.GetNumberOfJobs()};
  sup::dto::AnyValue temp_out;
  sup::protocol::FunctionProtocolPack(temp_out, kNumberOfJobsFieldName, number_jobs);
  if (!sup::dto::TryAssignIfEmptyOrConvert(output, temp_out))
  {
    return sup::protocol::ServerProtocolEncodingError;
  }
  return sup::protocol::Success;
}

sup::protocol::ProtocolResult AutomationServerProtocol::GetJobInfo(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  sup::dto::uint64 idx;
  auto result = ExtractJobIndex(input, idx);
  if (result != sup::protocol::Success)
  {
    return result;
  }
  const auto& job_info = m_auto_server.GetJobInfo(idx);
  auto job_info_av = ToAnyValue(job_info);
  sup::dto::AnyValue temp_out;
  sup::protocol::FunctionProtocolPack(temp_out, kJobInfoFieldName, job_info_av);
  if (!sup::dto::TryAssignIfEmptyOrConvert(output, temp_out))
  {
    return sup::protocol::ServerProtocolEncodingError;
  }
  return sup::protocol::Success;
}

sup::protocol::ProtocolResult AutomationServerProtocol::EditBreakpoint(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  (void)output;
  sup::dto::uint64 job_idx;
  auto result = ExtractJobIndex(input, job_idx);
  if (result != sup::protocol::Success)
  {
    return result;
  }
  sup::dto::uint64 instr_idx;
  auto number_of_instructions = m_auto_server.GetJobInfo(job_idx).GetNumberOfInstructions();
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
  m_auto_server.EditBreakpoint(job_idx, instr_idx, breakpoint_active);
  return sup::protocol::Success;
}

sup::protocol::ProtocolResult AutomationServerProtocol::SendJobCommand(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  (void)output;
  sup::dto::uint64 idx;
  auto result = ExtractJobIndex(input, idx);
  if (result != sup::protocol::Success)
  {
    return result;
  }
  sup::sequencer::JobCommand command{sup::sequencer::JobCommand::kStart};
  result = ExtractJobCommand(input, command);
  if (result != sup::protocol::Success)
  {
    return result;
  }
  m_auto_server.SendJobCommand(idx, command);
  return sup::protocol::Success;
}

sup::protocol::ProtocolResult AutomationServerProtocol::ExtractJobIndex(
  const sup::dto::AnyValue& input, sup::dto::uint64& idx)
{
  sup::dto::AnyValue idx_av;
  if (!sup::protocol::FunctionProtocolExtract(idx_av, input, kJobIndexFieldName))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  if (!idx_av.As(idx))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  if (idx >= m_auto_server.GetNumberOfJobs())
  {
    return UnknownJob;
  }
  return sup::protocol::Success;
}

sup::protocol::ProtocolResult AutomationServerProtocol::ExtractInstructionIndex(
  const sup::dto::AnyValue& input, std::size_t number_of_instructions, sup::dto::uint64& idx)
{
  sup::dto::AnyValue idx_av;
  if (!sup::protocol::FunctionProtocolExtract(idx_av, input, kInstructionIndexFieldName))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  if (!idx_av.As(idx))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  if (idx >= number_of_instructions)
  {
    return UnknownInstruction;
  }
  return sup::protocol::Success;
}

const sup::protocol::ProtocolResult NotSupported{status::kNotSupported};
const sup::protocol::ProtocolResult UnknownJob{status::kUnknownJob};
const sup::protocol::ProtocolResult UnknownInstruction{status::kUnknownInstruction};
const sup::protocol::ProtocolResult UnknownJobCommand{status::kUnknownJobCommand};

sup::dto::AnyValue ToAnyValue(const JobInfo& job_info)
{
  sup::dto::AnyValue result = {{
    { kJobPrefixFieldName, job_info.GetPrefix() },
    { kFullNameFieldName, job_info.GetProcedureName() },
    { kNumberOfVarsFieldName,
      { sup::dto::UnsignedInteger64Type, job_info.GetNumberOfVariables() } },
    { kNumberOfInstrFieldName,
      { sup::dto::UnsignedInteger64Type, job_info.GetNumberOfInstructions() } },
    { kInstructionTreeInfoFieldName, job_info.GetInstructionTreeInfo() }
  }};
  return result;
}

}  // namespace auto_server

}  // namespace sup

namespace
{
using namespace sup::auto_server;
sup::protocol::ProtocolResult ExtractJobCommand(const sup::dto::AnyValue& input,
                                                sup::sequencer::JobCommand& command)
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
  if (command_int >= static_cast<sup::dto::uint32>(sup::sequencer::JobCommand::kTerminate))
  {
    return UnknownJobCommand;
  }
  command = static_cast<sup::sequencer::JobCommand>(command_int);
  return sup::protocol::Success;
}

}  // unnamed namespace

