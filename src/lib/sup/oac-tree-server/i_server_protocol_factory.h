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

#ifndef SUP_OAC_TREE_SERVER_I_SERVER_PROTOCOL_FACTORY_H_
#define SUP_OAC_TREE_SERVER_I_SERVER_PROTOCOL_FACTORY_H_

#include <sup/oac-tree/i_job_info_io.h>

namespace sup
{
namespace oac_tree_server
{
class AutomationServer;

class IMainServer
{
public:
  virtual ~IMainServer();

  // No copy or move
  IMainServer(const IMainServer& other) = delete;
  IMainServer(IMainServer&& other) = delete;
  IMainServer& operator=(const IMainServer& other) = delete;
  IMainServer& operator=(IMainServer&& other) = delete;

protected:
  IMainServer() = default;
};

/**
  * @brief Interface for server protocol factory.
 */
class IServerProtocolFactory
{
public:
  virtual ~IServerProtocolFactory();

  IServerProtocolFactory(const IServerProtocolFactory& other) = delete;
  IServerProtocolFactory(IServerProtocolFactory&& other) = delete;
  IServerProtocolFactory& operator=(const IServerProtocolFactory& other) = delete;
  IServerProtocolFactory& operator=(IServerProtocolFactory&& other) = delete;


  /**
   * @brief Create a main server that exposes control and information requests for the given
   * automation server over the network.
   */
  virtual std::unique_ptr<IMainServer> CreateMainServer(const std::string& service_name,
                                                        AutomationServer& automation_server) = 0;

  /**
   * @brief Create a JobInfoIO object for the given job index and number of variables. The server
   * prefix is provided to allow the created JobInfoIO to determine the appropriate network
   * resources to use (e.g. topic names or service names).
   */
  virtual std::unique_ptr<sup::oac_tree::IJobInfoIO> CreateJobInfoIO(
    const std::string& server_prefix, sup::dto::uint32 job_idx, sup::dto::uint32 n_vars) = 0;

protected:
  IServerProtocolFactory() = default;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_I_SERVER_PROTOCOL_FACTORY_H_
