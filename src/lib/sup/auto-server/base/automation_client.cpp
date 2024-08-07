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

#include <sup/auto-server/automation_client.h>

namespace sup
{
namespace auto_server
{

AutomationClient::AutomationClient(IJobManager& job_manager, IJobInfoIORegistry& job_info_io_reg,
                                   const ListenerFactoryFunction& factory_func)
{
  (void)job_manager;
  (void)job_info_io_reg;
  (void)factory_func;
}

AutomationClient::~AutomationClient() = default;

}  // namespace auto_server

}  // namespace sup
