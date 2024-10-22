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

#include <sup/auto-server/automation_protocol_client.h>
#include <sup/auto-server/client_job.h>
#include <sup/auto-server/epics_io_client.h>

#include <sup/protocol/protocol_rpc_client.h>
#include <sup/epics/pv_access_rpc_client.h>

#include <chrono>
#include <thread>

using namespace sup::auto_server;

const std::string kAutoServerName = "CTRL-SUP-AUTO:TEST-SERVER";

sup::sequencer::IJobInfoIO& GetMyJobInfoIO();

int main(int argc, char* argv[])
{
  // typical RPC client stack:
  auto rpc_client_config = sup::epics::GetDefaultRPCClientConfig(kAutoServerName);
  sup::epics::PvAccessRPCClient pv_access_rpc_client{rpc_client_config};
  sup::protocol::ProtocolRPCClient protocol_rpc_client{pv_access_rpc_client};

  // Create client JobManager and query number of jobs served:
  AutomationProtocolClient auto_protocol_client{protocol_rpc_client};
  auto n_jobs = auto_protocol_client.GetNumberOfJobs();

  // Create a client job.
  auto& my_job_info_io = GetMyJobInfoIO();
  auto job_0 = CreateClientJob(auto_protocol_client, 0, EPICSIOCLientFactoryFunction, my_job_info_io);

  // Get instruction and variable info and start job 0.
  const auto& job_info = job_0->GetInfo();
  job_0->Start();

  // wait 10s
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Halt job 0
  job_0->Halt();
}
