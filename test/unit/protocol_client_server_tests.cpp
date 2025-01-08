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

#include <sup/auto-server/automation_protocol_client.h>
#include <sup/auto-server/control_protocol_server.h>
#include <sup/auto-server/info_protocol_server.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/instruction_map.h>
#include <sup/sequencer/job_info_utils.h>
#include <sup/sequencer/sequence_parser.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Exactly;
using ::testing::Return;

using namespace sup::auto_server;

const std::string kTestAutomationServiceName = "Test::ProtocolClientServer";

class ProtocolClientServerTest : public ::testing::Test
{
protected:
  ProtocolClientServerTest();
  virtual ~ProtocolClientServerTest();

  using StrictMockJobManager = ::testing::StrictMock<UnitTestHelper::MockJobManager>;
  StrictMockJobManager m_job_manager;
  InfoProtocolServer m_info_server;
  ControlProtocolServer m_control_server;
  AutomationProtocolClient m_client_job_manager;
};

TEST_F(ProtocolClientServerTest, GetServerPrefix)
{
  // Test GetServerPrefix over the protocol layer
  const std::string server_prefix = "StackTestServerPrefix";
  EXPECT_CALL(m_job_manager, GetServerPrefix()).Times(Exactly(1)).WillOnce(Return(server_prefix));
  auto prefix_reply = m_client_job_manager.GetServerPrefix();
  EXPECT_EQ(prefix_reply, server_prefix);
}

TEST_F(ProtocolClientServerTest, GetNumberOfJobs)
{
  // Test GetNumberOfJobs over the protocol layer
  const sup::dto::uint32 n_jobs = 42u;
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  auto n_jobs_reply = m_client_job_manager.GetNumberOfJobs();
  EXPECT_EQ(n_jobs_reply, n_jobs);
}

TEST_F(ProtocolClientServerTest, GetJobInfo)
{
  // Test GetJobInfo over the protocol layer

  // Build JobInfo
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  auto root = proc->RootInstruction();
  sup::sequencer::InstructionMap instr_map{root};
  auto job_info = sup::sequencer::utils::CreateJobInfo(*proc, instr_map);

  // Create stack and test
  const sup::dto::uint32 n_jobs = 42u;
  const sup::dto::uint32 job_id = 32u;
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  EXPECT_CALL(m_job_manager, GetJobInfo(job_id)).Times(Exactly(1)).WillOnce(Return(job_info));
  auto job_info_reply = m_client_job_manager.GetJobInfo(job_id);
  EXPECT_EQ(job_info_reply, job_info);
}

TEST_F(ProtocolClientServerTest, EditBreakpoint)
{
  // Build JobInfo
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  auto root = proc->RootInstruction();
  sup::sequencer::InstructionMap instr_map{root};
  auto job_info = sup::sequencer::utils::CreateJobInfo(*proc, instr_map);

  // Test EditBreakpoint over the protocol layer
  const sup::dto::uint32 n_jobs = 42u;
  const sup::dto::uint32 job_id = 32u;
  const sup::dto::uint32 instr_id = 1u;
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  EXPECT_CALL(m_job_manager, GetJobInfo(job_id)).Times(Exactly(1)).WillOnce(Return(job_info));
  EXPECT_CALL(m_job_manager, EditBreakpoint(job_id, instr_id, true)).Times(Exactly(1));
  m_client_job_manager.EditBreakpoint(job_id, instr_id, true);
}

TEST_F(ProtocolClientServerTest, SendJobCommand)
{
  // Test SendJobCommand over the protocol layer
  const sup::dto::uint32 n_jobs = 42u;
  const sup::dto::uint32 job_id = 32u;
  auto command = sup::sequencer::JobCommand::kStart;
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  EXPECT_CALL(m_job_manager, SendJobCommand(job_id, command)).Times(Exactly(1));
  m_client_job_manager.SendJobCommand(job_id, command);
}

ProtocolClientServerTest::ProtocolClientServerTest()
  : m_job_manager{}
  , m_info_server{m_job_manager}
  , m_control_server{m_job_manager}
  , m_client_job_manager{m_info_server, m_control_server}
{}

ProtocolClientServerTest::~ProtocolClientServerTest() = default;
