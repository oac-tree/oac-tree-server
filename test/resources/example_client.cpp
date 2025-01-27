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

#include <sup/oac-tree-server/client_job.h>
#include <sup/oac-tree-server/epics_config_utils.h>

#include <sup/protocol/protocol_rpc_client.h>

#include <chrono>
#include <thread>

using namespace sup::oac_tree_server;

const std::string kAutoServerName = "CTRL-SUP-AUTO:TEST-SERVER";

sup::oac_tree::IJobInfoIO& GetMyJobInfoIO();

int main(int argc, char* argv[])
{
  // EPICS automation job manager:
  auto automation_client = utils::CreateEPICSJobManager(kAutoServerName);
  auto n_jobs = automation_client->GetNumberOfJobs();

  // Create a client job.
  auto& my_job_info_io = GetMyJobInfoIO();
  auto job_0 = CreateClientJob(*automation_client, 0, utils::CreateEPICSIOClient, my_job_info_io);

  // Get instruction and variable info and start job 0.
  const auto& job_info = job_0->GetInfo();
  job_0->Start();

  // wait 10s
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Halt job 0
  job_0->Halt();
}
