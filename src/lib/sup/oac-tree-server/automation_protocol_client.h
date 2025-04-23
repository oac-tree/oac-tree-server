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

#ifndef SUP_OAC_TREE_SERVER_AUTOMATION_PROTOCOL_CLIENT_H_
#define SUP_OAC_TREE_SERVER_AUTOMATION_PROTOCOL_CLIENT_H_

#include <sup/oac-tree-server/i_job_manager.h>

#include <sup/protocol/protocol.h>

namespace sup
{
namespace oac_tree_server
{
/**
 * @brief AutomationProtocolClient.
 */
class AutomationProtocolClient : public IJobManager
{
public:
  explicit AutomationProtocolClient(sup::protocol::Protocol& info_protocol,
                                    sup::protocol::Protocol& control_protocol);
  virtual ~AutomationProtocolClient();

  std::string GetServerPrefix() const override;

  sup::dto::uint32 GetNumberOfJobs() const override;

  sup::oac_tree::JobInfo GetJobInfo(sup::dto::uint32 job_idx) const override;

  void EditBreakpoint(sup::dto::uint32 job_idx, sup::dto::uint32 instr_idx, bool breakpoint_active) override;

  void SendJobCommand(sup::dto::uint32 job_idx, sup::oac_tree::JobCommand command) override;

private:
  sup::protocol::Protocol& m_info_protocol;
  sup::protocol::Protocol& m_control_protocol;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_AUTOMATION_PROTOCOL_CLIENT_H_
