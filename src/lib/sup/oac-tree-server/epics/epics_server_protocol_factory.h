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
  * @brief Interface for server protocol factory.
 */
class EPICSServerProtocolFactory final : public IServerProtocolFactory
{
public:
  EPICSServerProtocolFactory(sup::dto::uint32 n_procs);
 ~EPICSServerProtocolFactory() override;

 std::unique_ptr<sup::oac_tree::IJobInfoIO> CreateJobInfoIO(
  const std::string& server_prefix, sup::dto::uint32 job_idx, sup::dto::uint32 n_vars) override;

private:
  EPICSAnyValueManagerRegistry m_av_mgr_registry;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_EPICS_SERVER_PROTOCOL_FACTORY_H_
