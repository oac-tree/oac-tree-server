/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : Unit test code
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
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

#include <sup/oac-tree-server/automation_server.h>
#include <sup/oac-tree-server/client_job.h>
#include <sup/oac-tree-server/control_protocol_server.h>
#include <sup/oac-tree-server/epics_config_utils.h>
#include <sup/oac-tree-server/info_protocol_server.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/epics/epics_protocol_factory.h>
#include <sup/oac-tree/instruction_map.h>
#include <sup/oac-tree/job_info_utils.h>
#include <sup/oac-tree/sequence_parser.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Exactly;
using ::testing::Return;
using ::testing::AtLeast;

using namespace sup::oac_tree_server;
using sup::oac_tree::JobState;
using sup::oac_tree::InstructionState;
using sup::oac_tree::ExecutionStatus;
using sup::oac_tree::JobCommand;

const std::string kTestAutomationServiceName = "Test::ClientJobTestsStack";
const std::string kTestServerPrefix = "ClientJobTestsPrefix";

class ClientJobTests : public ::testing::Test
{
protected:
  ClientJobTests();
  virtual ~ClientJobTests();

  UnitTestHelper::TestJobInfoIO m_job_info_io;
  std::unique_ptr<IJobManager> m_client_job_manager;

  static void SetUpTestSuite();
  static void TearDownTestSuite();

  static IAnyValueManagerRegistry& GetAnyValueManagerRegistry();

  static AutomationServer m_automation_server;
  static InfoProtocolServer m_info_server;
  static ControlProtocolServer m_control_server;
  static std::unique_ptr<sup::protocol::RPCServerInterface> m_info_stack;
  static std::unique_ptr<sup::protocol::RPCServerInterface> m_control_stack;
};

TEST_F(ClientJobTests, JobCommands)
{
  // Query information about the server and its job
  ASSERT_EQ(m_client_job_manager->GetNumberOfJobs(), 1u);
  sup::dto::uint32 job_id{0};
  auto job_0 = CreateClientJob(*m_client_job_manager, job_id, utils::CreateEPICSIOClient,
                               m_job_info_io);

  EXPECT_EQ(job_0->GetInfo().GetNumberOfVariables(), 3);
  EXPECT_EQ(job_0->GetInfo().GetNumberOfInstructions(), 5);
  EXPECT_EQ(job_0->GetInfo().GetProcedureName(), "Common header");

  EXPECT_NO_THROW(job_0->SetBreakpoint(0));
  EXPECT_NO_THROW(job_0->Start());
  EXPECT_NO_THROW(job_0->RemoveBreakpoint(0));
  EXPECT_NO_THROW(job_0->Step());
  EXPECT_NO_THROW(job_0->Start());
  EXPECT_NO_THROW(job_0->Pause());
  EXPECT_NO_THROW(job_0->Halt());
}

ClientJobTests::ClientJobTests()
  : m_job_info_io{}
  , m_client_job_manager{utils::CreateEPICSJobManager(kTestAutomationServiceName)}
{}

ClientJobTests::~ClientJobTests()
{
  // Ensure each test starts in an initial state of the job
  m_client_job_manager->SendJobCommand(0, JobCommand::kHalt);
  m_client_job_manager->SendJobCommand(0, JobCommand::kReset);
}

AutomationServer ClientJobTests::m_automation_server{
  kTestServerPrefix, ClientJobTests::GetAnyValueManagerRegistry()};
InfoProtocolServer ClientJobTests::m_info_server{m_automation_server};
ControlProtocolServer ClientJobTests::m_control_server{m_automation_server};
std::unique_ptr<sup::protocol::RPCServerInterface> ClientJobTests::m_info_stack{};
std::unique_ptr<sup::protocol::RPCServerInterface> ClientJobTests::m_control_stack{};

IAnyValueManagerRegistry& ClientJobTests::GetAnyValueManagerRegistry()
{
  static auto registry = utils::CreateEPICSAnyValueManagerRegistry(1);
  return *registry;
}

void ClientJobTests::SetUpTestSuite()
{
  sup::epics::PvAccessRPCServerConfig info_server_config{kTestAutomationServiceName};
  m_info_stack = sup::epics::CreateEPICSRPCServerStack(m_info_server, info_server_config);
  auto control_service_name = GetControlServerName(kTestAutomationServiceName);
  sup::epics::PvAccessRPCServerConfig control_server_config{control_service_name};
  m_control_stack = sup::epics::CreateEPICSRPCServerStack(m_control_server, control_server_config);

  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  m_automation_server.AddJob(std::move(proc));
}

void ClientJobTests::TearDownTestSuite()
{
  m_info_stack.reset();
  m_control_stack.reset();
}
