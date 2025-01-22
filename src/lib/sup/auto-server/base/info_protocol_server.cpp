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
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/auto-server/info_protocol_server.h>

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/protocol/function_protocol_extract.h>
#include <sup/protocol/function_protocol_pack.h>
#include <sup/protocol/protocol_rpc.h>
#include <sup/oac-tree/job_info_utils.h>

namespace sup
{
namespace auto_server
{

InfoProtocolServer::InfoProtocolServer(IJobManager& job_manager)
  : m_job_manager{job_manager}
{}

InfoProtocolServer::~InfoProtocolServer() = default;

sup::protocol::ProtocolResult InfoProtocolServer::Invoke(const sup::dto::AnyValue& input,
                                                               sup::dto::AnyValue& output)
{
  return sup::protocol::CallFunctionProtocol(*this, FunctionMap(), input, output, NotSupported);
}

sup::protocol::ProtocolResult InfoProtocolServer::Service(const sup::dto::AnyValue& input,
                                                                sup::dto::AnyValue& output)
{
  if (sup::protocol::utils::IsApplicationProtocolRequestPayload(input))
  {
    return sup::protocol::utils::HandleApplicationProtocolInfo(
      output, kAutomationInfoServerProtocolServerType, kAutomationInfoServerProtocolServerVersion);
  }
  return NotSupported;
}

const sup::protocol::ProtocolMemberFunctionMap<InfoProtocolServer>&
InfoProtocolServer::FunctionMap()
{
  static sup::protocol::ProtocolMemberFunctionMap<InfoProtocolServer> f_map = {
    { kGetServerPrefixFunctionName, &InfoProtocolServer::GetServerPrefix },
    { kGetNumberOfJobsFunctionName, &InfoProtocolServer::GetNumberOfJobs },
    { kGetJobInfoFunctionName, &InfoProtocolServer::GetJobInfo }
  };
  return f_map;
}

sup::protocol::ProtocolResult InfoProtocolServer::GetServerPrefix(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  (void)input;
  auto server_prefix = m_job_manager.GetServerPrefix();
  sup::dto::AnyValue temp_out;
  sup::protocol::FunctionProtocolPack(temp_out, kServerPrefixFieldName, server_prefix);
  if (!sup::dto::TryAssignIfEmptyOrConvert(output, temp_out))
  {
    return sup::protocol::ServerProtocolEncodingError;
  }
  return sup::protocol::Success;
}

sup::protocol::ProtocolResult InfoProtocolServer::GetNumberOfJobs(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  (void)input;
  sup::dto::AnyValue number_jobs{sup::dto::UnsignedInteger64Type, m_job_manager.GetNumberOfJobs()};
  sup::dto::AnyValue temp_out;
  sup::protocol::FunctionProtocolPack(temp_out, kNumberOfJobsFieldName, number_jobs);
  if (!sup::dto::TryAssignIfEmptyOrConvert(output, temp_out))
  {
    return sup::protocol::ServerProtocolEncodingError;
  }
  return sup::protocol::Success;
}

sup::protocol::ProtocolResult InfoProtocolServer::GetJobInfo(
  const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  sup::dto::uint32 idx{};
  auto result = ExtractJobIndex(input, m_job_manager.GetNumberOfJobs(), idx);
  if (result != sup::protocol::Success)
  {
    return result;
  }
  const auto& job_info = m_job_manager.GetJobInfo(idx);
  auto job_info_av = sup::oac_tree::utils::ToAnyValue(job_info);
  sup::dto::AnyValue temp_out;
  sup::protocol::FunctionProtocolPack(temp_out, kJobInfoFieldName, job_info_av);
  if (!sup::dto::TryAssignIfEmptyOrConvert(output, temp_out))
  {
    return sup::protocol::ServerProtocolEncodingError;
  }
  return sup::protocol::Success;
}

}  // namespace auto_server

}  // namespace sup
