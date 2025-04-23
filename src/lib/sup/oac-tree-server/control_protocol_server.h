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

#ifndef SUP_OAC_TREE_SERVER_CONTROL_PROTOCOL_SERVER_H_
#define SUP_OAC_TREE_SERVER_CONTROL_PROTOCOL_SERVER_H_

#include <sup/oac-tree-server/i_job_manager.h>

#include <sup/protocol/function_protocol.h>
#include <sup/protocol/protocol.h>

namespace sup
{
namespace oac_tree_server
{
/**
 * @brief ControlProtocolServer provides the server side protocol for requests to control a job. It
 * is separated from the server that handles information about a job for reasons of access control.
 */
class ControlProtocolServer : public sup::protocol::Protocol
{
public:
  explicit ControlProtocolServer(IJobManager& job_manager);
  ~ControlProtocolServer();

  sup::protocol::ProtocolResult Invoke(const sup::dto::AnyValue& input,
                                       sup::dto::AnyValue& output) override;
  sup::protocol::ProtocolResult Service(const sup::dto::AnyValue& input,
                                        sup::dto::AnyValue& output) override;

private:
  static const sup::protocol::ProtocolMemberFunctionMap<ControlProtocolServer>& FunctionMap();
  IJobManager& m_job_manager;
  sup::protocol::ProtocolResult EditBreakpoint(const sup::dto::AnyValue& input,
                                               sup::dto::AnyValue& output);
  sup::protocol::ProtocolResult SendJobCommand(const sup::dto::AnyValue& input,
                                               sup::dto::AnyValue& output);
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_CONTROL_PROTOCOL_SERVER_H_
