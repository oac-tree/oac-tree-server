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

#include <sup/oac-tree-server/epics/epics_client_job.h>

#include <sup/oac-tree-server/automation_server.h>
#include <sup/oac-tree-server/epics_utils.h>
#include <sup/oac-tree-server/exceptions.h>

#include <sup/oac-tree/instruction_map.h>
#include <sup/oac-tree/job_info_utils.h>
#include <sup/oac-tree/sequence_parser.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

#include <chrono>

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Exactly;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Throw;

using namespace sup::oac_tree_server;
using sup::oac_tree::ExecutionStatus;
using sup::oac_tree::InstructionState;
using sup::oac_tree::JobCommand;
using sup::oac_tree::JobState;

const std::string kTestAutomationServiceName = "Test::ClientJobTestsStack";
const std::string kTestServerPrefix = "ClientJobTestsPrefix";

//! Tests for the WaitForServerConnection helper used by the EPICS client job (no server required).

TEST(WaitForServerConnectionTest, ReturnsTrueWhenServerResponds)
{
  NiceMock<UnitTestHelper::MockJobManager> job_manager;
  EXPECT_CALL(job_manager, GetServerPrefix()).WillRepeatedly(Return("prefix"));

  EXPECT_TRUE(WaitForServerConnection(job_manager, std::chrono::milliseconds(500)));
}

TEST(WaitForServerConnectionTest, ReturnsFalseWhenServerUnreachable)
{
  NiceMock<UnitTestHelper::MockJobManager> job_manager;
  EXPECT_CALL(job_manager, GetServerPrefix())
      .WillRepeatedly(Throw(NetworkException("no connection to server")));

  const auto start = std::chrono::steady_clock::now();
  EXPECT_FALSE(WaitForServerConnection(job_manager, std::chrono::milliseconds(250)));
  const auto elapsed = std::chrono::steady_clock::now() - start;

  // The probe must respect the timeout instead of hanging on the failing requests.
  EXPECT_LT(elapsed, std::chrono::seconds(5));
}

//! Tests for the EPICS client job exercised against a real EPICS automation server.

class ClientJobTests : public ::testing::Test
{
protected:
  ClientJobTests();
  virtual ~ClientJobTests();

  UnitTestHelper::TestJobInfoIO m_job_info_io;
  std::unique_ptr<IClientJobManager> m_client_mgr;

  static void SetUpTestSuite();
  static void TearDownTestSuite();

  static std::unique_ptr<IServerProtocolFactory> m_server_factory;
  static std::unique_ptr<AutomationServer> m_automation_server;
  static std::unique_ptr<IMainServer> m_main_server;
};

TEST_F(ClientJobTests, ConstructorError)
{
  auto job = m_client_mgr->CreateJob(999, m_job_info_io);
  EXPECT_EQ(job.get(), nullptr);
}

TEST_F(ClientJobTests, JobCommands)
{
  // Query information about the server and its job
  ASSERT_EQ(m_client_mgr->GetNumberOfJobs(), 1u);
  sup::dto::uint32 job_id{0};
  auto job_0 = m_client_mgr->CreateJob(job_id, m_job_info_io);

  EXPECT_TRUE(job_0->WaitForConnection(std::chrono::milliseconds(1000)));

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
  EXPECT_NO_THROW(job_0->Reset());
}

ClientJobTests::ClientJobTests()
  : m_job_info_io{}
  , m_client_mgr{utils::CreateEPICSClientProtocolManager(kTestAutomationServiceName)}
{}

ClientJobTests::~ClientJobTests()
{
  // Ensure each test starts in an initial state of the job
  m_client_mgr->SendJobCommand(0, JobCommand::kHalt);
  m_client_mgr->SendJobCommand(0, JobCommand::kReset);
}

std::unique_ptr<IServerProtocolFactory> ClientJobTests::m_server_factory{};
std::unique_ptr<AutomationServer> ClientJobTests::m_automation_server{};
std::unique_ptr<IMainServer> ClientJobTests::m_main_server{};

void ClientJobTests::SetUpTestSuite()
{
  m_server_factory = utils::CreateEPICSServerProtocolFactory(kTestAutomationServiceName, 1);
  m_automation_server = std::make_unique<AutomationServer>(*m_server_factory);

  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  m_automation_server->AddJob(std::move(proc));

  m_main_server = m_server_factory->CreateMainServer(*m_automation_server);
}

void ClientJobTests::TearDownTestSuite()
{
  // Tear down the entire server stack so that no EPICS server keeps running in the background
  // after this suite, where it would interfere with subsequent EPICS tests. Order matters: the
  // main server references the automation server, which in turn references the factory's managers.
  m_main_server.reset();
  m_automation_server.reset();
  m_server_factory.reset();
}
