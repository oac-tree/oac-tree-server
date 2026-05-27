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

#ifndef SUP_OAC_TREE_SERVER_EPICS_SERVER_PROTOCOL_FACTORY_H_
#define SUP_OAC_TREE_SERVER_EPICS_SERVER_PROTOCOL_FACTORY_H_

#include <sup/oac-tree-server/i_server_protocol_factory.h>

#include "epics_anyvalue_manager_registry.h"

namespace sup
{
namespace oac_tree_server
{
/**
  * @brief Server protocol factory implementation that uses EPICS as the network backend.
 */
class EPICSServerProtocolFactory final : public IServerProtocolFactory
{
public:
  EPICSServerProtocolFactory(const std::string& service_name, sup::dto::uint32 n_managers);
 ~EPICSServerProtocolFactory() override;

  // No copy or move
  EPICSServerProtocolFactory(const EPICSServerProtocolFactory& other) = delete;
  EPICSServerProtocolFactory(EPICSServerProtocolFactory&& other) = delete;
  EPICSServerProtocolFactory& operator=(const EPICSServerProtocolFactory& other) = delete;
  EPICSServerProtocolFactory& operator=(EPICSServerProtocolFactory&& other) = delete;

  std::unique_ptr<IMainServer> CreateMainServer(IJobManager& job_manager) override;

  std::unique_ptr<sup::oac_tree::IJobInfoIO> CreateJobInfoIO(sup::dto::uint32 job_idx,
                                                             sup::dto::uint32 n_vars) override;

private:
  std::string m_service_name;
  EPICSAnyValueManagerRegistry m_av_mgr_registry;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_EPICS_SERVER_PROTOCOL_FACTORY_H_
