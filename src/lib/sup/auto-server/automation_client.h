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

#ifndef SUP_AUTO_SERVER_AUTOMATION_CLIENT_H_
#define SUP_AUTO_SERVER_AUTOMATION_CLIENT_H_

#include <sup/auto-server/job_info.h>

#include <sup/sequencer/job_commands.h>

#include <memory>
#include <string>
#include <vector>

namespace sup
{
namespace auto_server
{
/**
 * @brief AutomationClient.
 */
class AutomationClient
{
public:
  AutomationClient(const std::string& server_prefix);
  ~AutomationClient();

  std::string GetServerPrefix() const;
  std::size_t GetNumberOfJobs() const;

  // TODO: refactor so that both client and server implement the same interface
  // this may lead to removal of JobInfo??
  const JobInfo& GetJobProxy(std::size_t job_idx) const;

  void EditBreakpoint(std::size_t job_idx, std::size_t instr_idx, bool breakpoint_active);

  void SendJobCommand(std::size_t job_idx, sup::sequencer::JobCommand command);
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_AUTOMATION_CLIENT_H_
