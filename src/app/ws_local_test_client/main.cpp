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

#include "test_job_info_io.h"
#include "utils.h"

#include <sup/cli/command_line_parser.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace sup::oac_tree_server;

int main(int argc, char* argv[])
{
  sup::cli::CommandLineParser parser;
  parser.SetDescription(
      /*header*/ "",
      "The program launches a oac-tree client to test the connection to a server.");
  parser.AddHelpOption();

  parser.AddOption({"-p", "--port"}, "Port to use for web socket server")
      .SetParameter(true)
      .SetValueName("port")
      .SetRequired(true);

  if (!parser.Parse(argc, argv))
  {
    std::cout << parser.GetUsageString();
    return 0;
  }

  auto client_mgr = utils::CreateClientJobManager(parser);
  if (!client_mgr)
  {
    std::cout << parser.GetUsageString();
    return 0;
  }

  if (!client_mgr->WaitForConnection(std::chrono::seconds(1)))
  {
    std::cerr << "Failed to connect to server." << std::endl;
    return 1;
  }

  auto num_jobs = client_mgr->GetNumberOfJobs();
  std::cout << "Connected to server with " << num_jobs << " jobs." << std::endl;

  if (num_jobs > 0)
  {
    utils::TestJobInfoIO job_info_io;
    auto client_job = client_mgr->CreateJob(0, job_info_io);
    if (!client_job)
    {
      std::cerr << "Failed to create client job." << std::endl;
      return 1;
    }
    client_job->Start();
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  return 0;
}
