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

#ifndef SUP_OAC_TREE_SERVER_WS_CLIENT_JOB_H_
#define SUP_OAC_TREE_SERVER_WS_CLIENT_JOB_H_

#include <sup/oac-tree-server/i_job_manager.h>

#include <sup/oac-tree/i_job.h>
#include <sup/oac-tree/i_job_info_io.h>

#include <memory>

namespace sup
{
namespace oac_tree_server
{
class WSClientJobImpl;

/**
  * @brief Client job that uses WebSocket as the backend.
 */
class WSClientJob final : public sup::oac_tree::IJob
{
public:
  WSClientJob(IJobManager& job_manager, sup::dto::uint32 job_idx,
              const std::string& host_name, unsigned short port,
              sup::oac_tree::IJobInfoIO& job_info_io);
 ~WSClientJob() override;

  // Only move
  WSClientJob(WSClientJob&& other) noexcept;
  WSClientJob& operator=(WSClientJob&& other) noexcept;

  WSClientJob(const WSClientJob& other) = delete;
  WSClientJob& operator=(const WSClientJob& other) = delete;

  const sup::oac_tree::JobInfo& GetInfo() const override;
  void SetBreakpoint(sup::dto::uint32 instr_idx) override;
  void RemoveBreakpoint(sup::dto::uint32 instr_idx) override;
  void Start() override;
  void Step() override;
  void Pause() override;
  void Reset() override;
  void Halt() override;

private:
  std::unique_ptr<WSClientJobImpl> m_impl;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_CLIENT_JOB_H_
