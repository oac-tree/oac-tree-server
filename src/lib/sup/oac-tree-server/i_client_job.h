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

#ifndef SUP_OAC_TREE_SERVER_I_CLIENT_JOB_H_
#define SUP_OAC_TREE_SERVER_I_CLIENT_JOB_H_

#include <sup/oac-tree/i_job.h>

#include <chrono>

namespace sup
{
namespace oac_tree_server
{
/**
 * @brief Interface for a client job that supports waiting for a connection.
 */
class IClientJob : public sup::oac_tree::IJob
{
public:
  virtual ~IClientJob();

  IClientJob(const IClientJob& other) = delete;
  IClientJob(IClientJob&& other) = delete;
  IClientJob& operator=(const IClientJob& other) = delete;
  IClientJob& operator=(IClientJob&& other) = delete;

  /**
   * @brief Wait for a connection to be established within the specified timeout.
   * @param timeout The maximum duration in milliseconds to wait for a connection.
   * @return true if the connection was established within the timeout, false otherwise.
   */
  virtual bool WaitForConnection(std::chrono::milliseconds timeout) = 0;

protected:
  IClientJob() = default;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_I_CLIENT_JOB_H_
