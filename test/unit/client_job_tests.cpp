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

#include <sup/oac-tree-server/automation_server.h>
#include <sup/oac-tree-server/base/client_job.h>
#include <sup/oac-tree-server/epics_utils.h>

#include <sup/oac-tree-server/exceptions.h>

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
  std::unique_ptr<IClientProtocolManager> m_client_mgr;

  static void SetUpTestSuite();
  static void TearDownTestSuite();

  static IServerProtocolFactory& GetServerProtocolFactory();

  static AutomationServer m_automation_server;
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
  ASSERT_EQ(GetNumberOfJobs(*m_client_mgr), 1u);
  sup::dto::uint32 job_id{0};
  auto job_0 = m_client_mgr->CreateJob(job_id, m_job_info_io);

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
  m_client_mgr->GetJobManager().SendJobCommand(0, JobCommand::kHalt);
  m_client_mgr->GetJobManager().SendJobCommand(0, JobCommand::kReset);
}

AutomationServer ClientJobTests::m_automation_server{ClientJobTests::GetServerProtocolFactory()};
std::unique_ptr<IMainServer> ClientJobTests::m_main_server{};

IServerProtocolFactory& ClientJobTests::GetServerProtocolFactory()
{
  static auto factory = utils::CreateEPICSServerProtocolFactory(kTestAutomationServiceName, 1);
  return *factory;
}

void ClientJobTests::SetUpTestSuite()
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  m_automation_server.AddJob(std::move(proc));

  m_main_server = GetServerProtocolFactory().CreateMainServer(m_automation_server);
}

void ClientJobTests::TearDownTestSuite()
{
  m_main_server.reset();
}
