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

#include <sup/oac-tree-server/exceptions.h>
#include <sup/oac-tree-server/ws_utils.h>

#include <sup/oac-tree/instruction_map.h>
#include <sup/oac-tree/job_info_utils.h>
#include <sup/oac-tree/sequence_parser.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

using ::testing::Exactly;
using ::testing::Return;

using namespace sup::oac_tree_server;

namespace
{
constexpr unsigned short kTestWSPortBase = 35791u;
const std::string kTestHost = "localhost";
std::atomic<unsigned short> g_next_test_port{kTestWSPortBase};
}

class WSJobManagerTest : public ::testing::Test
{
protected:
  WSJobManagerTest();
  virtual ~WSJobManagerTest();

  using StrictMockJobManager = ::testing::StrictMock<UnitTestHelper::MockJobManager>;

  StrictMockJobManager m_job_manager;
  unsigned short m_test_port;
  std::unique_ptr<IServerProtocolFactory> m_server_factory;
  std::unique_ptr<IMainServer> m_main_server;
  std::unique_ptr<IClientJobManager> m_client_mgr;
};

TEST_F(WSJobManagerTest, GetServerPrefix)
{
  auto prefix_reply = m_client_mgr->GetServerPrefix();
  EXPECT_EQ(prefix_reply, kTestHost + ":" + std::to_string(m_test_port));
}

TEST_F(WSJobManagerTest, GetNumberOfJobs)
{
  const sup::dto::uint32 n_jobs = 42u;

  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  EXPECT_TRUE(m_client_mgr->WaitForConnection(std::chrono::milliseconds(1000)));
  EXPECT_EQ(m_client_mgr->GetNumberOfJobs(), n_jobs);
}

TEST_F(WSJobManagerTest, GetJobInfo)
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  auto root = proc->RootInstruction();
  sup::oac_tree::InstructionMap instr_map{root};
  auto job_info = sup::oac_tree::utils::CreateJobInfo(*proc, instr_map);
  const sup::dto::uint32 job_id = 32u;

  EXPECT_CALL(m_job_manager, GetJobInfo(job_id)).Times(Exactly(1)).WillOnce(Return(job_info));
  EXPECT_TRUE(m_client_mgr->WaitForConnection(std::chrono::milliseconds(1000)));
  EXPECT_EQ(m_client_mgr->GetJobInfo(job_id), job_info);
}

TEST_F(WSJobManagerTest, EditBreakpoint)
{
  const sup::dto::uint32 n_jobs = 42u;
  const sup::dto::uint32 job_id = 32u;
  const sup::dto::uint32 instr_id = 1u;

  using ::testing::InSequence;
  {
    InSequence s;
    EXPECT_CALL(m_job_manager, EditBreakpoint(job_id, instr_id, true)).Times(Exactly(1));
    EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  }
  EXPECT_TRUE(m_client_mgr->WaitForConnection(std::chrono::milliseconds(1000)));
  m_client_mgr->EditBreakpoint(job_id, instr_id, true);
  EXPECT_EQ(m_client_mgr->GetNumberOfJobs(), n_jobs);
}

TEST_F(WSJobManagerTest, SendJobCommand)
{
  const sup::dto::uint32 n_jobs = 42u;
  const sup::dto::uint32 job_id = 32u;
  auto command = sup::oac_tree::JobCommand::kStart;

  using ::testing::InSequence;
  {
    InSequence s;
    EXPECT_CALL(m_job_manager, SendJobCommand(job_id, command)).Times(Exactly(1));
    EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));
  }
  EXPECT_TRUE(m_client_mgr->WaitForConnection(std::chrono::milliseconds(1000)));
  m_client_mgr->SendJobCommand(job_id, command);
  EXPECT_EQ(m_client_mgr->GetNumberOfJobs(), n_jobs);
}

TEST_F(WSJobManagerTest, TwoClients)
{
  const sup::dto::uint32 n_jobs = 8u;

  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(3)).WillRepeatedly(Return(n_jobs));
  EXPECT_TRUE(m_client_mgr->WaitForConnection(std::chrono::milliseconds(1000)));
  EXPECT_EQ(m_client_mgr->GetNumberOfJobs(), n_jobs);

  auto new_client_mgr = utils::CreateWSClientJobManager(kTestHost, m_test_port);
  EXPECT_TRUE(new_client_mgr->WaitForConnection(std::chrono::milliseconds(1000)));
  EXPECT_EQ(new_client_mgr->GetNumberOfJobs(), n_jobs);

  // after destroying the first client, the second one should still be able to get the number of jobs
  m_client_mgr.release();
  EXPECT_EQ(new_client_mgr->GetNumberOfJobs(), n_jobs);
}

WSJobManagerTest::WSJobManagerTest()
  : m_job_manager{}
  , m_test_port{g_next_test_port.fetch_add(1u)}
  , m_server_factory{utils::CreateWSServerProtocolFactory(m_test_port)}
  , m_main_server{m_server_factory->CreateMainServer(m_job_manager)}
  , m_client_mgr{}
{
  m_client_mgr = utils::CreateWSClientJobManager(kTestHost, m_test_port);
}

WSJobManagerTest::~WSJobManagerTest()
{
  m_client_mgr.release();
  m_main_server.release();
  m_server_factory.release();
}
