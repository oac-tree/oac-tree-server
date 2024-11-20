/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVER
 *
 * Description   : Unit test code
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

#include <sup/auto-server/automation_server.h>
#include <sup/auto-server/client_job.h>
#include <sup/auto-server/control_protocol_server.h>
#include <sup/auto-server/epics_config_utils.h>
#include <sup/auto-server/info_protocol_server.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/epics/epics_protocol_factory.h>
#include <sup/sequencer/instruction_map.h>
#include <sup/sequencer/job_info_utils.h>
#include <sup/sequencer/sequence_parser.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Exactly;
using ::testing::Return;
using ::testing::AtLeast;

using namespace sup::auto_server;
using sup::sequencer::JobState;
using sup::sequencer::InstructionState;
using sup::sequencer::ExecutionStatus;
using sup::sequencer::JobCommand;

const std::string kTestAutomationServiceName = "Test::FullClientServerStack";
const std::string kTestServerPrefix = "FullStackServerPrefix";

class FullClientServerStackTest : public ::testing::Test
{
protected:
  FullClientServerStackTest();
  virtual ~FullClientServerStackTest();

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

TEST_F(FullClientServerStackTest, ServerInformation)
{
  // Query information about the server and its job
  EXPECT_EQ(m_client_job_manager->GetServerPrefix(), kTestServerPrefix);
  ASSERT_EQ(m_client_job_manager->GetNumberOfJobs(), 1u);
  sup::dto::uint32 job_id{0};
  auto job_info = m_client_job_manager->GetJobInfo(job_id);
  auto job_0 = CreateClientJob(*m_client_job_manager, job_id, utils::CreateEPICSIOClient,
                               m_job_info_io);
  EXPECT_TRUE(m_job_info_io.WaitForJobState(JobState::kInitial, 1.0));
  InstructionState initial{ false, ExecutionStatus::NOT_STARTED };
  EXPECT_TRUE(m_job_info_io.WaitForInstructionState(0, initial, 1.0));
  EXPECT_TRUE(m_job_info_io.WaitForInstructionState(1, initial, 1.0));
  EXPECT_TRUE(m_job_info_io.WaitForInstructionState(2, initial, 1.0));
  sup::dto::AnyValue zero_av{ sup::dto::UnsignedInteger32Type, 0 };
  sup::dto::AnyValue one_av{ sup::dto::UnsignedInteger32Type, 1 };
  EXPECT_TRUE(m_job_info_io.WaitForVariableValue(0, one_av, 1.0));
  EXPECT_TRUE(m_job_info_io.WaitForVariableValue(1, zero_av, 1.0));
  EXPECT_TRUE(m_job_info_io.WaitForVariableValue(2, zero_av, 1.0));
}

TEST_F(FullClientServerStackTest, JobCommands)
{
  // Query information about the server and its job
  ASSERT_EQ(m_client_job_manager->GetNumberOfJobs(), 1u);
  sup::dto::uint32 job_id{0};
  auto job_0 = CreateClientJob(*m_client_job_manager, job_id, utils::CreateEPICSIOClient,
                               m_job_info_io);

  EXPECT_TRUE(m_job_info_io.WaitForJobState(JobState::kInitial, 1.0));
  m_client_job_manager->SendJobCommand(job_id, JobCommand::kStart);
  EXPECT_TRUE(m_job_info_io.WaitForJobState(JobState::kSucceeded, 1.0));

  InstructionState success{ false, ExecutionStatus::SUCCESS };
  EXPECT_TRUE(m_job_info_io.WaitForInstructionState(0, success, 1.0));
  EXPECT_TRUE(m_job_info_io.WaitForInstructionState(1, success, 1.0));
  EXPECT_TRUE(m_job_info_io.WaitForInstructionState(2, success, 1.0));
  sup::dto::AnyValue zero_av{ sup::dto::UnsignedInteger32Type, 0 };
  sup::dto::AnyValue one_av{ sup::dto::UnsignedInteger32Type, 1 };
  EXPECT_TRUE(m_job_info_io.WaitForVariableValue(0, one_av, 1.0));
  EXPECT_TRUE(m_job_info_io.WaitForVariableValue(1, one_av, 1.0));
  EXPECT_TRUE(m_job_info_io.WaitForVariableValue(2, one_av, 1.0));
}

FullClientServerStackTest::FullClientServerStackTest()
  : m_job_info_io{}
  , m_client_job_manager{utils::CreateEPICSJobManager(kTestAutomationServiceName)}
{}

FullClientServerStackTest::~FullClientServerStackTest()
{
  // Ensure each test starts in an initial state of the job
  m_client_job_manager->SendJobCommand(0, JobCommand::kHalt);
  m_client_job_manager->SendJobCommand(0, JobCommand::kReset);
}

AutomationServer FullClientServerStackTest::m_automation_server{
  kTestServerPrefix, FullClientServerStackTest::GetAnyValueManagerRegistry()};
InfoProtocolServer FullClientServerStackTest::m_info_server{m_automation_server};
ControlProtocolServer FullClientServerStackTest::m_control_server{m_automation_server};
std::unique_ptr<sup::protocol::RPCServerInterface> FullClientServerStackTest::m_info_stack{};
std::unique_ptr<sup::protocol::RPCServerInterface> FullClientServerStackTest::m_control_stack{};

IAnyValueManagerRegistry& FullClientServerStackTest::GetAnyValueManagerRegistry()
{
  static auto registry = utils::CreateEPICSAnyValueManagerRegistry(1);
  return *registry;
}

void FullClientServerStackTest::SetUpTestSuite()
{
  sup::epics::PvAccessRPCServerConfig info_server_config{kTestAutomationServiceName};
  m_info_stack = sup::epics::CreateEPICSRPCServerStack(m_info_server, info_server_config);
  auto control_service_name = GetControlServerName(kTestAutomationServiceName);
  sup::epics::PvAccessRPCServerConfig control_server_config{control_service_name};
  m_control_stack = sup::epics::CreateEPICSRPCServerStack(m_control_server, control_server_config);

  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  m_automation_server.AddJob(std::move(proc));
}

void FullClientServerStackTest::TearDownTestSuite()
{
  m_info_stack.reset();
  m_control_stack.reset();
}
