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
 * Copyright (c) : 2010-2026 ITER Organization,
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

#ifndef SUP_OAC_TREE_SERVER_WS_CLIENT_JOB_MANAGER_H_
#define SUP_OAC_TREE_SERVER_WS_CLIENT_JOB_MANAGER_H_

#include <sup/oac-tree-server/i_client_job_manager.h>

#include <memory>

namespace sup
{
namespace oac_tree_server
{
class WSJobManager;

/**
  * @brief Client job manager implementation that uses WebSocket as the network backend.
 */
class WSClientJobManager final : public IClientJobManager
{
public:
  WSClientJobManager(const std::string& host_name, unsigned short port);
 ~WSClientJobManager() override;

  // No copy or move
  WSClientJobManager(const WSClientJobManager& other) = delete;
  WSClientJobManager(WSClientJobManager&& other) = delete;
  WSClientJobManager& operator=(const WSClientJobManager& other) = delete;
  WSClientJobManager& operator=(WSClientJobManager&& other) = delete;

  std::string GetServerPrefix() const override;

  sup::dto::uint32 GetNumberOfJobs() const override;

  sup::oac_tree::JobInfo GetJobInfo(sup::dto::uint32 job_idx) const override;

  void EditBreakpoint(sup::dto::uint32 job_idx, sup::dto::uint32 instr_idx,
                              bool breakpoint_active) override;

  void SendJobCommand(sup::dto::uint32 job_idx, sup::oac_tree::JobCommand command) override;

  std::unique_ptr<IClientJob> CreateJob(
    sup::dto::uint32 job_idx, sup::oac_tree::IJobInfoIO &job_info_io) override;

  bool WaitForConnection(std::chrono::milliseconds timeout) override;

private:
  std::string m_host_name;
  unsigned short m_port;
  std::unique_ptr<WSJobManager> m_job_manager;
};

std::unique_ptr<WSJobManager> CreateWSJobManager(const std::string& host_name, unsigned short port);

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_CLIENT_JOB_MANAGER_H_
