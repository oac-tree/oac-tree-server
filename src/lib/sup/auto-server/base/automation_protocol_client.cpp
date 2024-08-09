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

#include <sup/auto-server/automation_protocol_client.h>

#include <sup/auto-server/anyvalue_utils.h>
#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/job_utils.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/protocol/function_protocol.h>
#include <sup/protocol/function_protocol_extract.h>
#include <sup/protocol/function_protocol_pack.h>

namespace sup
{
namespace auto_server
{
AutomationProtocolClient::AutomationProtocolClient(sup::protocol::Protocol& protocol)
  : m_protocol{protocol}
{}

AutomationProtocolClient::~AutomationProtocolClient() = default;

std::string AutomationProtocolClient::GetServerPrefix() const
{
  auto input = sup::protocol::FunctionProtocolInput(kGetServerPrefixFunctionName);
  sup::dto::AnyValue output;
  auto protocol_result = m_protocol.Invoke(input, output);
  if (protocol_result != sup::protocol::Success)
  {
    const std::string error = "AutomationProtocolClient::GetServerPrefix(): protocol did not return"
      " success: " + AutomationServerResultToString(protocol_result);
    throw InvalidOperationException(error);
  }
  std::string result;
  if (!sup::protocol::FunctionProtocolExtract(result, output, kServerPrefixFieldName))
  {
    const std::string error = "AutomationProtocolClient::GetServerPrefix(): could not extract "
      "server prefix from server reply";
    throw InvalidOperationException(error);
  }
  return result;
}

std::size_t AutomationProtocolClient::GetNumberOfJobs() const
{
  auto input = sup::protocol::FunctionProtocolInput(kGetNumberOfJobsFunctionName);
  sup::dto::AnyValue output;
  auto protocol_result = m_protocol.Invoke(input, output);
  if (protocol_result != sup::protocol::Success)
  {
    const std::string error = "AutomationProtocolClient::GetNumberOfJobs(): protocol did not return"
      " success: " + AutomationServerResultToString(protocol_result);
    throw InvalidOperationException(error);
  }
  sup::dto::AnyValue result;
  if (!sup::protocol::FunctionProtocolExtract(result, output, kNumberOfJobsFieldName)
      || result.GetType() != sup::dto::UnsignedInteger64Type)
  {
    const std::string error = "AutomationProtocolClient::GetNumberOfJobs(): could not extract "
      "number of jobs from server reply";
    throw InvalidOperationException(error);
  }
  return result.As<sup::dto::uint64>();
}

JobInfo AutomationProtocolClient::GetJobInfo(std::size_t job_idx) const
{
  auto input = sup::protocol::FunctionProtocolInput(kGetJobInfoFunctionName);
  sup::dto::AnyValue job_idx_av{sup::dto::UnsignedInteger64Type, job_idx};
  sup::protocol::FunctionProtocolPack(input, kJobIndexFieldName, job_idx_av);
  sup::dto::AnyValue output;
  auto protocol_result = m_protocol.Invoke(input, output);
  if (protocol_result != sup::protocol::Success)
  {
    const std::string error = "AutomationProtocolClient::GetJobInfo(): protocol did not return"
      " success: " + AutomationServerResultToString(protocol_result);
    throw InvalidOperationException(error);
  }
  sup::dto::AnyValue job_info_av;
  if (!sup::protocol::FunctionProtocolExtract(job_info_av, output, kJobInfoFieldName))
  {
    const std::string error = "AutomationProtocolClient::GetJobInfo(): could not extract "
      "job info from server reply";
    throw InvalidOperationException(error);
  }
  try
  {
    auto job_info = utils::ToJobInfo(job_info_av);
    return job_info;
  }
  catch(const InvalidOperationException&)
  {
    const std::string error = "AutomationProtocolClient::GetJobInfo(): could not convert received "
      "AnyValue to JobInfo";
    throw InvalidOperationException(error);
  }
}

void AutomationProtocolClient::EditBreakpoint(std::size_t job_idx, std::size_t instr_idx,
                                              bool breakpoint_active)
{
  auto input = sup::protocol::FunctionProtocolInput(kEditBreakpointCommandFunctionName);
  sup::dto::AnyValue job_idx_av{sup::dto::UnsignedInteger64Type, job_idx};
  sup::protocol::FunctionProtocolPack(input, kJobIndexFieldName, job_idx_av);
  sup::dto::AnyValue instr_idx_av{sup::dto::UnsignedInteger64Type, instr_idx};
  sup::protocol::FunctionProtocolPack(input, kInstructionIndexFieldName, instr_idx_av);
  sup::protocol::FunctionProtocolPack(input, kBreakpointActiveFieldName, breakpoint_active);
  sup::dto::AnyValue output;
  auto protocol_result = m_protocol.Invoke(input, output);
  if (protocol_result != sup::protocol::Success)
  {
    const std::string error = "AutomationProtocolClient::EditBreakpoint(): protocol did not return"
      " success: " + AutomationServerResultToString(protocol_result);
    throw InvalidOperationException(error);
  }
}

void AutomationProtocolClient::SendJobCommand(std::size_t job_idx, sup::sequencer::JobCommand command)
{
  auto input = sup::protocol::FunctionProtocolInput(kSendJobCommandFunctionName);
  sup::dto::AnyValue job_idx_av{sup::dto::UnsignedInteger64Type, job_idx};
  sup::protocol::FunctionProtocolPack(input, kJobIndexFieldName, job_idx_av);
  auto command_int = static_cast<sup::dto::uint32>(command);
  sup::dto::AnyValue command_av{sup::dto::UnsignedInteger32Type, command_int};
  sup::protocol::FunctionProtocolPack(input, kJobCommandFieldName, command_av);
  sup::dto::AnyValue output;
  auto protocol_result = m_protocol.Invoke(input, output);
  if (protocol_result != sup::protocol::Success)
  {
    const std::string error = "AutomationProtocolClient::SendJobCommand(): protocol did not return"
      " success: " + AutomationServerResultToString(protocol_result);
    throw InvalidOperationException(error);
  }
}

}  // namespace auto_server

}  // namespace sup
