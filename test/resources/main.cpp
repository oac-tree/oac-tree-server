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
#include <sup/auto-server/automation_client.h>
#include <sup/auto-server/epics_anyvalue_listener.h>

#include <sup/protocol/protocol_rpc_client.h>
#include <sup/epics/pv_access_rpc_client.h>

#include <chrono>
#include <thread>

using namespace sup::auto_server;

const std::string kAutoServerName = "CTRL-SUP-AUTO:TEST-SERVER";

IJobInfoIO& GetMyJobInfoIO();

int main(int argc, char* argv[])
{
  // typical RPC client stack:
  auto rpc_client_config = sup::epics::GetDefaultRPCClientConfig(kAutoServerName);
  sup::epics::PvAccessRPCClient pv_access_rpc_client{rpc_client_config};
  sup::protocol::ProtocolRPCClient protocol_rpc_client{pv_access_rpc_client};

  // automation client classes:
  AutomationProtocolClient auto_protocol_client{protocol_rpc_client};
  AutomationClient auto_client{auto_protocol_client, EPICSListenerFactoryFunction};

  // Use AutomationClient to connect a IJobInfoIO object to a specific job.
  // This will make that object handle all job updates and user I/O.
  auto& my_job_info_io = GetMyJobInfoIO();
  auto_client.Connect(0, my_job_info_io);

  // AutomationProtocolClient implements IJobManager, so use that to query static info:
  // Get number of jobs:
  auto n_jobs = auto_protocol_client.GetNumberOfJobs();
  // Get instruction and variable information for job 0 (always a ref)
  const auto& job_0_info = auto_protocol_client.GetJobInfo(0);
  // Start job 0
  auto_protocol_client.SendJobCommand(0, sup::sequencer::JobCommand::kStart);

  // wait 10s
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Halt job 0
  auto_protocol_client.SendJobCommand(0, sup::sequencer::JobCommand::kHalt);
}
