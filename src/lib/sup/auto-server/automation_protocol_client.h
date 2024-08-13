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

#ifndef SUP_AUTO_SERVER_AUTOMATION_PROTOCOL_CLIENT_H_
#define SUP_AUTO_SERVER_AUTOMATION_PROTOCOL_CLIENT_H_

#include <sup/auto-server/i_job_manager.h>

#include <sup/protocol/protocol.h>

namespace sup
{
namespace auto_server
{
/**
 * @brief AutomationProtocolClient.
 */
class AutomationProtocolClient : public IJobManager
{
public:
  explicit AutomationProtocolClient(sup::protocol::Protocol& protocol);
  virtual ~AutomationProtocolClient();

  std::string GetServerPrefix() const override;

  std::size_t GetNumberOfJobs() const override;

  JobInfo GetJobInfo(std::size_t job_idx) const override;

  void EditBreakpoint(std::size_t job_idx, std::size_t instr_idx, bool breakpoint_active) override;

  void SendJobCommand(std::size_t job_idx, sup::sequencer::JobCommand command) override;

private:
  sup::protocol::Protocol& m_protocol;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_AUTOMATION_PROTOCOL_CLIENT_H_
