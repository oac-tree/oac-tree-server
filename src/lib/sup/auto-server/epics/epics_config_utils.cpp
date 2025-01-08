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
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "epics_io_client.h"

#include <sup/auto-server/automation_client_stack.h>
#include <sup/auto-server/epics/epics_anyvalue_manager_registry.h>
#include <sup/auto-server/sup_auto_protocol.h>
#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/pv_access_rpc_client.h>

namespace sup
{
namespace auto_server
{
namespace utils
{

std::unique_ptr<IAnyValueIO> CreateEPICSIOClient(IAnyValueManager& av_mgr)
{
  return std::unique_ptr<IAnyValueIO>(new EPICSIOClient(av_mgr));
}

std::unique_ptr<IJobManager> CreateEPICSJobManager(const std::string& server_name)
{
  auto info_config = sup::epics::GetDefaultRPCClientConfig(server_name);
  auto info_protocol = sup::epics::CreateEPICSRPCClientStack(info_config);
  auto control_server_name = GetControlServerName(server_name);
  auto control_config = sup::epics::GetDefaultRPCClientConfig(control_server_name);
  auto control_protocol = sup::epics::CreateEPICSRPCClientStack(control_config);
  std::unique_ptr<IJobManager> result{
      new AutomationClientStack{std::move(info_protocol), std::move(control_protocol)}};
  return result;
}

std::unique_ptr<IAnyValueManagerRegistry> CreateEPICSAnyValueManagerRegistry(
    sup::dto::uint32 n_managers)
{
  std::unique_ptr<IAnyValueManagerRegistry> result{new EPICSAnyValueManagerRegistry{n_managers}};
  return result;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
