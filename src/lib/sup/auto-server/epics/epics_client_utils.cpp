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

#include "epics_io_client.h"

#include <sup/auto-server/automation_client_stack.h>

#include <sup/epics/pv_access_rpc_client.h>
#include <sup/epics/epics_protocol_factory.h>

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
  auto rpc_client_config = sup::epics::GetDefaultRPCClientConfig(server_name);
  auto rpc_client_protocol = sup::epics::CreateEPICSRPCClientStack(rpc_client_config);
  std::unique_ptr<IJobManager> result{new AutomationClientStack{std::move(rpc_client_protocol)}};
  return result;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
