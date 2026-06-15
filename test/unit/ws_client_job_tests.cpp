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
#include <sup/oac-tree-server/websocket/ws_client_context.h>
#include <sup/oac-tree-server/websocket/ws_client_job.h>
#include <sup/oac-tree-server/ws_utils.h>

#include <sup/oac-tree/instruction_map.h>
#include <sup/oac-tree/job_info_utils.h>
#include <sup/oac-tree/sequence_parser.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

using ::testing::Exactly;
using ::testing::Return;

using namespace sup::oac_tree_server;

using sup::oac_tree::ExecutionStatus;
using sup::oac_tree::InstructionState;
using sup::oac_tree::JobState;

namespace
{
constexpr unsigned short kTestWSPortBase = 35821u;
const std::string kTestHost = "localhost";
std::atomic<unsigned short> g_next_test_port{kTestWSPortBase};

sup::oac_tree::JobInfo BuildJobInfo()
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::oac_tree::ParseProcedureString(procedure_string);
  EXPECT_NE(proc.get(), nullptr);
  auto root = proc->RootInstruction();
  sup::oac_tree::InstructionMap instr_map{root};
  return sup::oac_tree::utils::CreateJobInfo(*proc, instr_map);
}

class RecordingJobInfoIO : public sup::oac_tree::IJobInfoIO
{
public:
  void InitNumberOfInstructions(sup::dto::uint32 n_instr) override
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_n_instr = n_instr;
    }
    m_cv.notify_all();
  }

  void InstructionStateUpdated(sup::dto::uint32 instr_idx,
                               sup::oac_tree::InstructionState state) override
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_instr_states[instr_idx] = state;
    }
    m_cv.notify_all();
  }

  void BreakpointInstructionUpdated(sup::dto::uint32 instr_idx) override
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_bp_instr = instr_idx;
    }
    m_cv.notify_all();
  }

  void VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                       bool connected) override
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_var_connected[var_idx] = connected;
      if (connected)
      {
        m_var_values[var_idx] = value;
      }
    }
    m_cv.notify_all();
  }

  void JobStateUpdated(sup::oac_tree::JobState state) override
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_job_state = state;
    }
    m_cv.notify_all();
  }

  void PutValue(const sup::dto::AnyValue& value, const std::string& description) override
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_put_value = value;
      m_put_description = description;
    }
    m_cv.notify_all();
  }

  bool GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                    const std::string& description) override
  {
    (void)id;
    (void)value;
    (void)description;
    return false;
  }

  int GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                    const sup::dto::AnyValue& metadata) override
  {
    (void)id;
    (void)options;
    (void)metadata;
    return -1;
  }

  void Interrupt(sup::dto::uint64 id) override
  {
    (void)id;
  }

  void Message(const std::string& message) override
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_message = message;
    }
    m_cv.notify_all();
  }

  void Log(int severity, const std::string& message) override
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      ++m_log_count;
      m_log_severity = severity;
      m_log_message = message;
    }
    m_cv.notify_all();
  }

  void ProcedureTicked() override
  {}

  bool WaitForInstructionState(sup::dto::uint32 instr_idx,
                               sup::oac_tree::InstructionState state, double seconds)
  {
    return WaitFor([this, instr_idx, state]() {
      auto iter = m_instr_states.find(instr_idx);
      return iter != m_instr_states.end() && iter->second == state;
    }, seconds);
  }

  bool WaitForBreakpointInstruction(sup::dto::uint32 instr_idx, double seconds)
  {
    return WaitFor([this, instr_idx]() { return m_bp_instr == instr_idx; }, seconds);
  }

  bool WaitForVariableValue(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                            bool connected, double seconds)
  {
    return WaitFor([this, var_idx, value, connected]() {
      auto connected_it = m_var_connected.find(var_idx);
      auto value_it = m_var_values.find(var_idx);
      return connected_it != m_var_connected.end() && connected_it->second == connected
        && value_it != m_var_values.end() && value_it->second == value;
    }, seconds);
  }

  bool WaitForJobState(sup::oac_tree::JobState state, double seconds)
  {
    return WaitFor([this, state]() { return m_job_state == state; }, seconds);
  }

  bool WaitForPutValue(const sup::dto::AnyValue& value, const std::string& description,
                       double seconds)
  {
    return WaitFor([this, value, description]() {
      return m_put_value == value && m_put_description == description;
    }, seconds);
  }

  bool WaitForMessage(const std::string& message, double seconds)
  {
    return WaitFor([this, message]() { return m_message == message; }, seconds);
  }

  bool WaitForLog(int severity, const std::string& message, double seconds)
  {
    return WaitFor([this, severity, message]() {
      return m_log_severity == severity && m_log_message == message;
    }, seconds);
  }

  bool WaitForAnyLog(double seconds)
  {
    return WaitFor([this]() { return m_log_count > 0; }, seconds);
  }

  int GetLastLogSeverity() const
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    return m_log_severity;
  }

  std::string GetLastLogMessage() const
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    return m_log_message;
  }

private:
  template <typename Predicate>
  bool WaitFor(Predicate pred, double seconds)
  {
    auto duration = std::chrono::duration<double>(seconds);
    std::unique_lock<std::mutex> lk{m_mtx};
    return m_cv.wait_for(lk, duration, pred);
  }

  sup::dto::uint32 m_n_instr{0u};
  sup::dto::uint32 m_bp_instr{sup::oac_tree::kInvalidInstructionIndex};
  std::map<sup::dto::uint32, sup::oac_tree::InstructionState> m_instr_states;
  std::map<sup::dto::uint32, sup::dto::AnyValue> m_var_values;
  std::map<sup::dto::uint32, bool> m_var_connected;
  sup::oac_tree::JobState m_job_state{sup::oac_tree::JobState::kInitial};
  sup::dto::AnyValue m_put_value{};
  std::string m_put_description;
  std::string m_message;
  int m_log_count{0};
  int m_log_severity{0};
  std::string m_log_message;
  mutable std::mutex m_mtx;
  std::condition_variable m_cv;
};

}  // namespace

class WSClientJobConstructorTest : public ::testing::Test
{
protected:
  using StrictMockJobManager = ::testing::StrictMock<UnitTestHelper::MockJobManager>;

  StrictMockJobManager m_job_manager;
  UnitTestHelper::TestJobInfoIO m_job_info_io;
  std::shared_ptr<WSClientContext> m_context{std::make_shared<WSClientContext>()};
};

TEST_F(WSClientJobConstructorTest, ConstructorThrowsWhenJobIndexOutOfRange)
{
  const sup::dto::uint32 n_jobs = 3u;
  const sup::dto::uint32 invalid_job_idx = 5u;

  EXPECT_CALL(m_job_manager, GetServerPrefix())
      .Times(Exactly(1))
      .WillOnce(Return(kTestHost + ":1"));
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));

  EXPECT_THROW(
      {
        WSClientJob job(m_job_manager, invalid_job_idx, m_job_info_io, m_context);
      },
      InvalidOperationException);
}

TEST_F(WSClientJobConstructorTest, ConstructorThrowsWhenJobIndexEqualToNumberOfJobs)
{
  const sup::dto::uint32 n_jobs = 5u;
  const sup::dto::uint32 invalid_job_idx = 5u;

  EXPECT_CALL(m_job_manager, GetServerPrefix())
      .Times(Exactly(1))
      .WillOnce(Return(kTestHost + ":1"));
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(n_jobs));

  EXPECT_THROW(
      {
        WSClientJob job(m_job_manager, invalid_job_idx, m_job_info_io, m_context);
      },
      InvalidOperationException);
}

TEST_F(WSClientJobConstructorTest, MoveConstructionAndAssignment)
{
  const sup::dto::uint32 n_jobs = 2u;
  auto job_info = BuildJobInfo();
  const auto n_vars = job_info.GetNumberOfVariables();

  EXPECT_CALL(m_job_manager, GetServerPrefix()).WillRepeatedly(Return(kTestHost + ":1"));
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).WillRepeatedly(Return(n_jobs));
  EXPECT_CALL(m_job_manager, GetJobInfo(::testing::_)).WillRepeatedly(Return(job_info));

  WSClientJob job1(m_job_manager, 0u, m_job_info_io, m_context);

  // Move construction transfers the implementation; the target stays usable.
  WSClientJob job2(std::move(job1));
  EXPECT_EQ(job2.GetInfo().GetNumberOfVariables(), n_vars);

  // Move assignment transfers the implementation into an existing job.
  WSClientJob job3(m_job_manager, 1u, m_job_info_io, m_context);
  job3 = std::move(job2);
  EXPECT_EQ(job3.GetInfo().GetNumberOfVariables(), n_vars);
}

class WSClientJobWebSocketTest : public ::testing::Test
{
protected:
  WSClientJobWebSocketTest();
  ~WSClientJobWebSocketTest() override;

  using StrictMockJobManager = ::testing::StrictMock<UnitTestHelper::MockJobManager>;

  StrictMockJobManager m_job_manager;
  RecordingJobInfoIO m_client_job_info_io;
  unsigned short m_test_port;
  sup::oac_tree::JobInfo m_job_info;
  std::unique_ptr<IServerProtocolFactory> m_server_factory;
  std::unique_ptr<sup::oac_tree::IJobInfoIO> m_server_job_info_io;
  std::unique_ptr<IMainServer> m_main_server;
  std::shared_ptr<WSClientContext> m_context;
  std::unique_ptr<WSClientJob> m_client_job;
};

TEST_F(WSClientJobWebSocketTest, GetInfoReturnsFetchedJobInfo)
{
  EXPECT_EQ(m_client_job->GetInfo().GetNumberOfVariables(), m_job_info.GetNumberOfVariables());
  EXPECT_EQ(m_client_job->GetInfo().GetNumberOfInstructions(),
            m_job_info.GetNumberOfInstructions());
  EXPECT_EQ(m_client_job->GetInfo().GetProcedureName(), m_job_info.GetProcedureName());
}

TEST_F(WSClientJobWebSocketTest, ReceivesJobStateUpdateOverWebSocket)
{
  const auto expected_state = JobState::kRunning;

  m_server_job_info_io->JobStateUpdated(expected_state);

  EXPECT_TRUE(m_client_job_info_io.WaitForJobState(expected_state, 1.0));
}

TEST_F(WSClientJobWebSocketTest, ReceivesInstructionStateUpdateOverWebSocket)
{
  const sup::dto::uint32 instr_idx = 2u;
  const InstructionState state{true, ExecutionStatus::SUCCESS};

  m_server_job_info_io->InstructionStateUpdated(instr_idx, state);

  EXPECT_TRUE(m_client_job_info_io.WaitForInstructionState(instr_idx, state, 1.0));
}

TEST_F(WSClientJobWebSocketTest, ReceivesBreakpointInstructionUpdateOverWebSocket)
{
  const sup::dto::uint32 instr_idx = 3u;

  m_server_job_info_io->BreakpointInstructionUpdated(instr_idx);

  EXPECT_TRUE(m_client_job_info_io.WaitForBreakpointInstruction(instr_idx, 1.0));
}

TEST_F(WSClientJobWebSocketTest, ReceivesVariableUpdateOverWebSocket)
{
  const sup::dto::uint32 var_idx = 1u;
  const sup::dto::AnyValue value{sup::dto::UnsignedInteger64Type, 42u};

  m_server_job_info_io->VariableUpdated(var_idx, value, true);

  EXPECT_TRUE(m_client_job_info_io.WaitForVariableValue(var_idx, value, true, 1.0));
}

TEST_F(WSClientJobWebSocketTest, ReceivesPutValueUpdateOverWebSocket)
{
  const sup::dto::AnyValue value{sup::dto::BooleanType, true};
  const std::string description = "updated output";

  m_server_job_info_io->PutValue(value, description);

  EXPECT_TRUE(m_client_job_info_io.WaitForPutValue(value, description, 1.0));
}

TEST_F(WSClientJobWebSocketTest, ReceivesMessageUpdateOverWebSocket)
{
  const std::string message = "hello websocket client";

  m_server_job_info_io->Message(message);

  EXPECT_TRUE(m_client_job_info_io.WaitForMessage(message, 1.0));
}

TEST_F(WSClientJobWebSocketTest, ReceivesLogUpdateOverWebSocket)
{
  const int severity = 4;
  const std::string message = "websocket log entry";

  m_server_job_info_io->Log(severity, message);

  ASSERT_TRUE(m_client_job_info_io.WaitForAnyLog(1.0));
  EXPECT_EQ(m_client_job_info_io.GetLastLogSeverity(), severity);
  EXPECT_EQ(m_client_job_info_io.GetLastLogMessage(), message);
}

TEST_F(WSClientJobWebSocketTest, JobCommandsForwardedToJobManager)
{
  using sup::oac_tree::JobCommand;
  const sup::dto::uint32 instr_idx = 0u;

  EXPECT_CALL(m_job_manager, EditBreakpoint(0u, instr_idx, true)).Times(Exactly(1));
  EXPECT_CALL(m_job_manager, EditBreakpoint(0u, instr_idx, false)).Times(Exactly(1));
  EXPECT_CALL(m_job_manager, SendJobCommand(0u, JobCommand::kStart)).Times(Exactly(1));
  EXPECT_CALL(m_job_manager, SendJobCommand(0u, JobCommand::kStep)).Times(Exactly(1));
  EXPECT_CALL(m_job_manager, SendJobCommand(0u, JobCommand::kPause)).Times(Exactly(1));
  EXPECT_CALL(m_job_manager, SendJobCommand(0u, JobCommand::kReset)).Times(Exactly(1));
  EXPECT_CALL(m_job_manager, SendJobCommand(0u, JobCommand::kHalt)).Times(Exactly(1));

  m_client_job->SetBreakpoint(instr_idx);
  m_client_job->RemoveBreakpoint(instr_idx);
  m_client_job->Start();
  m_client_job->Step();
  m_client_job->Pause();
  m_client_job->Reset();
  m_client_job->Halt();
}

WSClientJobWebSocketTest::WSClientJobWebSocketTest()
  : m_job_manager{}
  , m_client_job_info_io{}
  , m_test_port{g_next_test_port.fetch_add(1u)}
  , m_job_info{BuildJobInfo()}
  , m_server_factory{utils::CreateWSServerProtocolFactory(m_test_port)}
  , m_server_job_info_io{m_server_factory->CreateJobInfoIO(0u, m_job_info.GetNumberOfVariables())}
  , m_main_server{}
  , m_context{std::make_shared<WSClientContext>()}
  , m_client_job{}
{
  m_main_server = m_server_factory->CreateMainServer(m_job_manager);

  EXPECT_CALL(m_job_manager, GetServerPrefix())
      .Times(Exactly(1))
      .WillOnce(Return(kTestHost + ":" + std::to_string(m_test_port)));
  EXPECT_CALL(m_job_manager, GetNumberOfJobs()).Times(Exactly(1)).WillOnce(Return(1u));
  EXPECT_CALL(m_job_manager, GetJobInfo(0u)).Times(Exactly(1)).WillOnce(Return(m_job_info));

  m_client_job = std::make_unique<WSClientJob>(m_job_manager, 0u, m_client_job_info_io, m_context);
  EXPECT_TRUE(m_client_job->WaitForConnection(std::chrono::milliseconds(1000)));

  // The client reports "connected" as soon as it receives the WebSocket handshake response, but the
  // server only registers the connection on its job endpoint when that response's write completes
  // (SimpleWeb runs connection_open from the async_write completion handler). Job updates published
  // in that window are broadcast to zero connections and silently dropped. Synchronise by
  // repeatedly publishing a sentinel job state until the client observes it: once any update is
  // received, the connection is registered and every later single publish is guaranteed to arrive.
  const auto sentinel_state = JobState::kPaused;
  bool updates_flow = false;
  const auto sync_deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (!updates_flow && std::chrono::steady_clock::now() < sync_deadline)
  {
    m_server_job_info_io->JobStateUpdated(sentinel_state);
    updates_flow = m_client_job_info_io.WaitForJobState(sentinel_state, 0.05);
  }
  EXPECT_TRUE(updates_flow);
}

WSClientJobWebSocketTest::~WSClientJobWebSocketTest()
{
  m_client_job.release();
  m_main_server.release();
  m_server_job_info_io.release();
  m_server_factory.release();
}

