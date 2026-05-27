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

#ifndef SUP_OAC_TREE_SERVER_EPICS_MAIN_SERVER_H_
#define SUP_OAC_TREE_SERVER_EPICS_MAIN_SERVER_H_

#include <sup/oac-tree-server/i_server_protocol_factory.h>

#include <sup/protocol/protocol_factory.h>

#include <memory>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief EPICSMainServer is the EPICS implementation of an RPC server that exposes control and
 * information requests for an automation server.
 */
class EPICSMainServer final : public IMainServer
{
public:
  explicit EPICSMainServer(const std::string& service_name, AutomationServer& automation_server);
  ~EPICSMainServer();

  // No copy or move
  EPICSMainServer(const EPICSMainServer& other) = delete;
  EPICSMainServer(EPICSMainServer&& other) = delete;
  EPICSMainServer& operator=(const EPICSMainServer& other) = delete;
  EPICSMainServer& operator=(EPICSMainServer&& other) = delete;

private:
  std::unique_ptr<sup::protocol::RPCServerInterface> m_info_server_stack;
  std::unique_ptr<sup::protocol::RPCServerInterface> m_control_server_stack;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_EPICS_MAIN_SERVER_H_
