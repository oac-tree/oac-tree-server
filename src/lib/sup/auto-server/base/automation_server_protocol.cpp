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

namespace sup
{
namespace auto_server
{

namespace status
{
enum AutomationServerStatus
{
  NOT_SUPPORTED = sup::protocol::SPECIFIC_APPLICATION_ERROR_START,
  UNKNOWN_JOB
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
    { kGetJobInfoFunctionName, &AutomationServerProtocol::GetJobInfo }
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
  sup::dto::AnyValue idx_av;
  if (!sup::protocol::FunctionProtocolExtract(idx_av, input, kJobIndexFieldName))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  sup::dto::uint64 idx;
  if (!idx_av.As(idx))
  {
    return sup::protocol::ServerProtocolDecodingError;
  }
  sup::dto::AnyValue job_info_av;
  try
  {
    auto job_info = m_auto_server.GetJobInfo(idx);
    job_info_av = ToAnyValue(job_info);
  }
  catch(const std::exception& e)
  {
    return UnknownJob;
  }
  sup::dto::AnyValue temp_out;
  sup::protocol::FunctionProtocolPack(temp_out, kJobInfoFieldName, job_info_av);
  if (!sup::dto::TryAssignIfEmptyOrConvert(output, temp_out))
  {
    return sup::protocol::ServerProtocolEncodingError;
  }
  return sup::protocol::Success;
}

const sup::protocol::ProtocolResult NotSupported{status::NOT_SUPPORTED};
const sup::protocol::ProtocolResult UnknownJob{status::UNKNOWN_JOB};

sup::dto::AnyValue ToAnyValue(const JobInfo& job_info)
{
  sup::dto::AnyValue result = {{
    { kJobPrefixFieldName, job_info.GetPrefix() },
    { kFullNameFieldName, job_info.GetProcedureName() },
    { kNumberOfVarsFieldName, { sup::dto::UnsignedInteger64Type, job_info.GetNumberOfVariables() } }
  }};
  return result;
}


}  // namespace auto_server

}  // namespace sup
