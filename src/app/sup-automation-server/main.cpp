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

#include "utils.h"

#include <sup/auto-server/automation_server_protocol.h>

#include <sup/cli/command_line_parser.h>
#include <sup/epics/epics_protocol_factory.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace sup::auto_server;

int main(int argc, char* argv[])
{
  sup::cli::CommandLineParser parser;
  parser.SetDescription(
      /*header*/ "",
      "The program launches a SUP automation server with the provided procedures.");
  parser.AddHelpOption();

  parser.AddOption({"-S", "--service"}, "Name of the automation server")
      .SetParameter(true)
      .SetValueName("service_name")
      .SetRequired(true);

  if (!parser.Parse(argc, argv))
  {
    std::cout << parser.GetUsageString();
    return 0;
  }
  auto proc_list = utils::GetProcedureList(parser);
  auto service_name = parser.GetValue<std::string>("--service");

  AutomationServerProtocol server_protocol{service_name, proc_list};
  sup::epics::PvAccessRPCServerConfig server_config{service_name};
  auto server_stack = sup::epics::CreateEPICSRPCServerStack(server_protocol, server_config);

  while(true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
