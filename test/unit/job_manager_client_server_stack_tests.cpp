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

using namespace sup::oac_tree_server;

const std::string kTestAutomationServiceName = "Test::JobManagerClientServerStack";


/**
 * @brief This IJobManager just forwards everything to a member IJobManager. It is required to
 * be able to combine global servers with local mock job managers.
 */
class ForwardingJobManager : public IJobManager
{
public:
  ForwardingJobManager() : m_job_manager{} {}
  virtual ~ForwardingJobManager() = default;

  void SetJobManager(IJobManager* job_manager)
  {
    m_job_manager = job_manager;
  }

  std::string GetServerPrefix() const override
  {
    return m_job_manager->GetServerPrefix();
  }

  sup::dto::uint32 GetNumberOfJobs() const override
  {
    return m_job_manager->GetNumberOfJobs();
  }

  sup::oac_tree::JobInfo GetJobInfo(sup::dto::uint32 job_idx) const override
  {
    return m_job_manager->GetJobInfo(job_idx);
  }

  void EditBreakpoint(sup::dto::uint32 job_idx, sup::dto::uint32 instr_idx,
                      bool breakpoint_active) override
  {
    m_job_manager->EditBreakpoint(job_idx, instr_idx, breakpoint_active);
  }

  void SendJobCommand(sup::dto::uint32 job_idx, sup::oac_tree::JobCommand command) override
  {
    m_job_manager->SendJobCommand(job_idx, command);
  }
private:
  IJobManager* m_job_manager;
};


class JobManagerClientServerStackTest : public ::testing::Test
{
protected:
  JobManagerClientServerStackTest();
  virtual ~JobManagerClientServerStackTest();

  using StrictMockJobManager = ::testing::StrictMock<UnitTestHelper::MockJobManager>;
  StrictMockJobManager m_job_manager;
  std::unique_ptr<IJobManager> m_client_job_manager;

  static void SetUpTestSuite();
  static void TearDownTestSuite();

  static ForwardingJobManager m_forwarding_job_manager;
  static InfoProtocolServer m_info_server;
  static ControlProtocolServer m_control_server;
  static std::unique_ptr<sup::protocol::RPCServerInterface> m_info_stack;
  static std::unique_ptr<sup::protocol::RPCServerInterface> m_control_stack;
};

TEST_F(JobManagerClientServerStackTest, GetServerPrefix)
{
  // Test GetServerPrefix over the whole EPICS stack
  const std::string server_prefix = "StackTestServerPrefix";
  EXPECT_CALL(m_job_manager, GetServerPrefix()).Times(Exactly(1)).WillOnce(Return(server_prefix));
  auto prefix_reply = m_client_job_manager->GetServerPrefix();
  EXPECT_EQ(prefix_reply, server_prefix);
}

TEST_F(JobManagerClientServerStackTest, GetNumberOfJobs)
{
  // Test GetNumberOfJobs over the whole EPICS stack
  const sup::dto::uint32 n_jobs = 42u;
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  auto n_jobs_reply = m_client_job_manager->GetNumberOfJobs();
  EXPECT_EQ(n_jobs_reply, n_jobs);
}

TEST_F(JobManagerClientServerStackTest, GetJobInfo)
{
  // Test GetJobInfo over the whole EPICS stack

  // Build JobInfo
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  auto root = proc->RootInstruction();
  sup::oac_tree::InstructionMap instr_map{root};
  auto job_info = sup::oac_tree::utils::CreateJobInfo(*proc, instr_map);

  // Create stack and test
  const sup::dto::uint32 n_jobs = 42u;
  const sup::dto::uint32 job_id = 32u;
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  EXPECT_CALL(m_job_manager, GetJobInfo(job_id)).Times(Exactly(1)).WillOnce(Return(job_info));
  auto job_info_reply = m_client_job_manager->GetJobInfo(job_id);
  EXPECT_EQ(job_info_reply, job_info);
}

TEST_F(JobManagerClientServerStackTest, EditBreakpoint)
{
  // Build JobInfo
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  auto root = proc->RootInstruction();
  sup::oac_tree::InstructionMap instr_map{root};
  auto job_info = sup::oac_tree::utils::CreateJobInfo(*proc, instr_map);

  // Test EditBreakpoint over the whole EPICS stack
  const sup::dto::uint32 n_jobs = 42u;
  const sup::dto::uint32 job_id = 32u;
  const sup::dto::uint32 instr_id = 1u;
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  EXPECT_CALL(m_job_manager, GetJobInfo(job_id)).Times(Exactly(1)).WillOnce(Return(job_info));
  EXPECT_CALL(m_job_manager, EditBreakpoint(job_id, instr_id, true)).Times(Exactly(1));
  m_client_job_manager->EditBreakpoint(job_id, instr_id, true);
}

TEST_F(JobManagerClientServerStackTest, SendJobCommand)
{
  // Test SendJobCommand over the whole EPICS stack
  const sup::dto::uint32 n_jobs = 42u;
  const sup::dto::uint32 job_id = 32u;
  auto command = sup::oac_tree::JobCommand::kStart;
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  EXPECT_CALL(m_job_manager, SendJobCommand(job_id, command)).Times(Exactly(1));
  m_client_job_manager->SendJobCommand(job_id, command);
}

JobManagerClientServerStackTest::JobManagerClientServerStackTest()
  : m_job_manager{}
  , m_client_job_manager{utils::CreateEPICSJobManager(kTestAutomationServiceName)}
{
  m_forwarding_job_manager.SetJobManager(std::addressof(m_job_manager));
}

JobManagerClientServerStackTest::~JobManagerClientServerStackTest()
{
  m_forwarding_job_manager.SetJobManager(nullptr);
}

ForwardingJobManager JobManagerClientServerStackTest::m_forwarding_job_manager{};
InfoProtocolServer JobManagerClientServerStackTest::m_info_server{m_forwarding_job_manager};
ControlProtocolServer JobManagerClientServerStackTest::m_control_server{m_forwarding_job_manager};
std::unique_ptr<sup::protocol::RPCServerInterface> JobManagerClientServerStackTest::m_info_stack{};
std::unique_ptr<sup::protocol::RPCServerInterface> JobManagerClientServerStackTest::m_control_stack{};

void JobManagerClientServerStackTest::SetUpTestSuite()
{
  sup::epics::PvAccessRPCServerConfig info_server_config{kTestAutomationServiceName};
  m_info_stack = sup::epics::CreateEPICSRPCServerStack(m_info_server, info_server_config);
  auto control_service_name = GetControlServerName(kTestAutomationServiceName);
  sup::epics::PvAccessRPCServerConfig control_server_config{control_service_name};
  m_control_stack = sup::epics::CreateEPICSRPCServerStack(m_control_server, control_server_config);
}

void JobManagerClientServerStackTest::TearDownTestSuite()
{
  m_info_stack.reset();
  m_control_stack.reset();
}
