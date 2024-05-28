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

#ifndef SUP_AUTO_SERVER_AUTOMATION_SERVER_PROTOCOL_H_
#define SUP_AUTO_SERVER_AUTOMATION_SERVER_PROTOCOL_H_

#include <sup/auto-server/automation_server.h>

#include <sup/protocol/function_protocol.h>
#include <sup/protocol/protocol.h>

#include <memory>
#include <string>

namespace sup
{
namespace auto_server
{
/**
 * @brief AutomationServerProtocol.
 */
class AutomationServerProtocol : public sup::protocol::Protocol
{
public:
  AutomationServerProtocol(const std::string& server_prefix,
                           AutomationServer::ProcedureList& proc_list);
  ~AutomationServerProtocol();

  sup::protocol::ProtocolResult Invoke(const sup::dto::AnyValue& input,
                                       sup::dto::AnyValue& output) override;
  sup::protocol::ProtocolResult Service(const sup::dto::AnyValue& input,
                                        sup::dto::AnyValue& output) override;

private:
  static const sup::protocol::ProtocolMemberFunctionMap<AutomationServerProtocol>& FunctionMap();
  const AutomationServer m_auto_server;
  sup::protocol::ProtocolResult GetServerPrefix(const sup::dto::AnyValue& input,
                                                sup::dto::AnyValue& output);
  sup::protocol::ProtocolResult GetNumberOfJobs(const sup::dto::AnyValue& input,
                                                sup::dto::AnyValue& output);
  sup::protocol::ProtocolResult GetJobInfo(const sup::dto::AnyValue& input,
                                           sup::dto::AnyValue& output);
};

/**
 * @brief The requested function is not supported.
*/
extern const sup::protocol::ProtocolResult NotSupported;
/**
 * @brief The requested job is unknown.
*/
extern const sup::protocol::ProtocolResult UnknownJob;

/**
 * @brief Encode a JobInfo object into an AnyValue.
*/
sup::dto::AnyValue ToAnyValue(const JobInfo& job_info);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_AUTOMATION_SERVER_PROTOCOL_H_
