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

#include "utils.h"

#include <sup/oac-tree-server/control_protocol_server.h>
#include <sup/oac-tree-server/info_protocol_server.h>
#include <sup/oac-tree-server/automation_server.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>
#include <sup/oac-tree-server/epics_config_utils.h>

#include <sup/cli/command_line_parser.h>
#include <sup/epics/epics_protocol_factory.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace sup::oac_tree_server;

int main(int argc, char* argv[])
{
  sup::cli::CommandLineParser parser;
  parser.SetDescription(
      /*header*/ "",
      "The program launches a oac-tree server with the provided procedures.");
  parser.AddHelpOption();

  parser.AddOption({"-s", "--service"}, "Name of the automation server")
      .SetParameter(true)
      .SetValueName("service_name")
      .SetRequired(true);

  parser.AddOption({"-d", "--dir"}, "Directory containing files with xml extension to be parsed and run")
      .SetParameter(true)
      .SetValueName("directory_name");

  parser.AddPositionalOption("FILE...", "File(s) to be parsed and run as procedures");

  if (!parser.Parse(argc, argv))
  {
    std::cout << parser.GetUsageString();
    return 0;
  }

  auto proc_list = utils::GetProcedureList(parser);
  auto service_name = parser.GetValue<std::string>("--service");
  auto anyvalue_manager_registry = utils::CreateEPICSAnyValueManagerRegistry(proc_list.size());

  AutomationServer auto_server{service_name, *anyvalue_manager_registry};
  for (auto& proc : proc_list)
  {
    auto_server.AddJob(std::move(proc));
  }
  // Instantiate RPC server for obtaining job information
  InfoProtocolServer info_server_protocol{auto_server};
  sup::epics::PvAccessRPCServerConfig info_server_config{service_name};
  auto info_server_stack = sup::epics::CreateEPICSRPCServerStack(info_server_protocol,
                                                                 info_server_config);
  // Instantiate RPC server for controlling jobs
  ControlProtocolServer control_server_protocol{auto_server};
  auto control_service_name = GetControlServerName(service_name);
  sup::epics::PvAccessRPCServerConfig control_server_config{control_service_name};
  auto control_server_stack = sup::epics::CreateEPICSRPCServerStack(control_server_protocol,
                                                                    control_server_config);

  while(true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
