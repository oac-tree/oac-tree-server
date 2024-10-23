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

#ifndef SUP_AUTO_SERVER_INFO_PROTOCOL_SERVER_H_
#define SUP_AUTO_SERVER_INFO_PROTOCOL_SERVER_H_

#include <sup/auto-server/i_job_manager.h>

#include <sup/protocol/function_protocol.h>
#include <sup/protocol/protocol.h>

namespace sup
{
namespace auto_server
{
/**
 * @brief InfoProtocolServer provides the server side protocol for requests to get information about
 * a job. It is separated from the server that handles controlling a job for reasons of access
 * control.
 */
class InfoProtocolServer : public sup::protocol::Protocol
{
public:
  explicit InfoProtocolServer(IJobManager& job_manager);
  ~InfoProtocolServer();

  sup::protocol::ProtocolResult Invoke(const sup::dto::AnyValue& input,
                                       sup::dto::AnyValue& output) override;
  sup::protocol::ProtocolResult Service(const sup::dto::AnyValue& input,
                                        sup::dto::AnyValue& output) override;

private:
  static const sup::protocol::ProtocolMemberFunctionMap<InfoProtocolServer>& FunctionMap();
  IJobManager& m_job_manager;
  sup::protocol::ProtocolResult GetServerPrefix(const sup::dto::AnyValue& input,
                                                sup::dto::AnyValue& output);
  sup::protocol::ProtocolResult GetNumberOfJobs(const sup::dto::AnyValue& input,
                                                sup::dto::AnyValue& output);
  sup::protocol::ProtocolResult GetJobInfo(const sup::dto::AnyValue& input,
                                           sup::dto::AnyValue& output);
  sup::protocol::ProtocolResult EditBreakpoint(const sup::dto::AnyValue& input,
                                               sup::dto::AnyValue& output);
  sup::protocol::ProtocolResult SendJobCommand(const sup::dto::AnyValue& input,
                                               sup::dto::AnyValue& output);
  sup::protocol::ProtocolResult ExtractJobIndex(const sup::dto::AnyValue& input,
                                                sup::dto::uint64& idx);
  sup::protocol::ProtocolResult ExtractInstructionIndex(const sup::dto::AnyValue& input,
                                                        std::size_t number_of_instructions,
                                                        sup::dto::uint64& idx);
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INFO_PROTOCOL_SERVER_H_
