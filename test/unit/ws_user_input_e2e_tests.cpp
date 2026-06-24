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
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

using ::testing::Exactly;
using ::testing::Return;

using namespace sup::oac_tree_server;

using sup::oac_tree::JobState;

namespace
{
constexpr unsigned short kTestWSPortBase = 36211u;
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

// Client-side IJobInfoIO acting as a controllable user interface. GetUserValue/GetUserChoice either
// return a preset answer immediately, or (in blocking mode) wait until released or interrupted, to
// mimic a dialog that stays open until the server cancels the request.
class UserInputClientIO : public sup::oac_tree::IJobInfoIO
{
public:
  void SetValueReply(const sup::dto::AnyValue& value)
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_value_reply = value;
  }
  void SetChoiceReply(int choice)
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_choice_reply = choice;
  }
  void SetBlocking(bool blocking)
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_blocking = blocking;
  }

  bool WaitForValuePrompt(double seconds)
  {
    return WaitFor([this] { return m_value_prompted; }, seconds);
  }
  bool WaitForInterrupt(double seconds)
  {
    return WaitFor([this] { return m_interrupt_called; }, seconds);
  }
  bool WaitForJobState(JobState state, double seconds)
  {
    return WaitFor([this, state] { return m_job_state == state; }, seconds);
  }

  bool GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                    const std::string& description) override
  {
    (void)id;
    (void)description;
    std::unique_lock<std::mutex> lk{m_mtx};
    m_value_prompted = true;
    m_interrupted = false;
    m_cv.notify_all();
    if (m_blocking)
    {
      m_cv.wait(lk, [this] { return m_interrupted; });
      return false;
    }
    value = m_value_reply;
    return true;
  }

  int GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                    const sup::dto::AnyValue& metadata) override
  {
    (void)id;
    (void)options;
    (void)metadata;
    std::unique_lock<std::mutex> lk{m_mtx};
    m_choice_prompted = true;
    m_interrupted = false;
    m_cv.notify_all();
    if (m_blocking)
    {
      m_cv.wait(lk, [this] { return m_interrupted; });
      return -1;
    }
    return m_choice_reply;
  }

  void Interrupt(sup::dto::uint64 id) override
  {
    (void)id;
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_interrupt_called = true;
      m_interrupted = true;
    }
    m_cv.notify_all();
  }

  void JobStateUpdated(JobState state) override
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_job_state = state;
    }
    m_cv.notify_all();
  }

  // Unused IJobInfoIO methods.
  void InitNumberOfInstructions(sup::dto::uint32) override {}
  void InstructionStateUpdated(sup::dto::uint32, sup::oac_tree::InstructionState) override {}
  void BreakpointInstructionUpdated(sup::dto::uint32) override {}
  void VariableUpdated(sup::dto::uint32, const sup::dto::AnyValue&, bool) override {}
  void PutValue(const sup::dto::AnyValue&, const std::string&) override {}
  void Message(const std::string&) override {}
  void Log(int, const std::string&) override {}
  void ProcedureTicked() override {}

private:
  template <typename Predicate>
  bool WaitFor(Predicate pred, double seconds)
  {
    auto duration = std::chrono::duration<double>(seconds);
    std::unique_lock<std::mutex> lk{m_mtx};
    return m_cv.wait_for(lk, duration, pred);
  }

  mutable std::mutex m_mtx;
  std::condition_variable m_cv;
  bool m_blocking{false};
  bool m_interrupted{false};
  bool m_value_prompted{false};
  bool m_choice_prompted{false};
  bool m_interrupt_called{false};
  JobState m_job_state{JobState::kInitial};
  sup::dto::AnyValue m_value_reply{};
  int m_choice_reply{-1};
};

}  // namespace

class WSUserInputE2ETest : public ::testing::Test
{
protected:
  WSUserInputE2ETest();
  ~WSUserInputE2ETest() override;

  using StrictMockJobManager = ::testing::StrictMock<UnitTestHelper::MockJobManager>;

  StrictMockJobManager m_job_manager;
  UserInputClientIO m_client_io;
  unsigned short m_test_port;
  sup::oac_tree::JobInfo m_job_info;
  std::unique_ptr<IServerProtocolFactory> m_server_factory;
  std::unique_ptr<sup::oac_tree::IJobInfoIO> m_server_job_info_io;
  std::unique_ptr<IMainServer> m_main_server;
  std::shared_ptr<WSClientContext> m_context;
  std::unique_ptr<WSClientJob> m_client_job;
};

TEST_F(WSUserInputE2ETest, GetUserValueRoundTrip)
{
  const sup::dto::uint64 id = 1u;
  const sup::dto::AnyValue provided{sup::dto::SignedInteger32Type, 7};
  m_client_io.SetValueReply(provided);

  sup::dto::AnyValue value{sup::dto::SignedInteger32Type, 0};
  auto future = std::async(std::launch::async, [this, id, &value] {
    return m_server_job_info_io->GetUserValue(id, value, "please provide a value");
  });

  ASSERT_EQ(future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
  EXPECT_TRUE(future.get());
  EXPECT_EQ(value, provided);
}

TEST_F(WSUserInputE2ETest, GetUserChoiceRoundTrip)
{
  const sup::dto::uint64 id = 2u;
  m_client_io.SetChoiceReply(2);

  const std::vector<std::string> options{"normal", "degraded", "fault"};
  auto future = std::async(std::launch::async, [this, id, &options] {
    return m_server_job_info_io->GetUserChoice(id, options, sup::dto::AnyValue{});
  });

  ASSERT_EQ(future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
  EXPECT_EQ(future.get(), 2);
}

TEST_F(WSUserInputE2ETest, ServerInterruptDismissesClientDialog)
{
  const sup::dto::uint64 id = 3u;
  m_client_io.SetBlocking(true);  // the client keeps its dialog open

  sup::dto::AnyValue value{sup::dto::SignedInteger32Type, 0};
  auto future = std::async(std::launch::async, [this, id, &value] {
    return m_server_job_info_io->GetUserValue(id, value, "please provide a value");
  });

  // The client is now showing its (blocking) dialog.
  ASSERT_TRUE(m_client_io.WaitForValuePrompt(2.0));

  // The server interrupts the request: it returns false and broadcasts a cancellation, which the
  // client uses to dismiss the open dialog.
  m_server_job_info_io->Interrupt(id);

  ASSERT_EQ(future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
  EXPECT_FALSE(future.get());
  EXPECT_TRUE(m_client_io.WaitForInterrupt(2.0));
}

WSUserInputE2ETest::WSUserInputE2ETest()
  : m_job_manager{}
  , m_client_io{}
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

  m_client_job = std::make_unique<WSClientJob>(m_job_manager, 0u, m_client_io, m_context);
  EXPECT_TRUE(m_client_job->WaitForConnection(std::chrono::milliseconds(1000)));

  // Wait until the server has registered the client connection on its job endpoint by repeatedly
  // publishing a sentinel job state until the client observes it (see WSClientJobWebSocketTest for
  // the detailed rationale). Only then are single publishes guaranteed to reach the client.
  const auto sentinel_state = JobState::kPaused;
  bool updates_flow = false;
  const auto sync_deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (!updates_flow && std::chrono::steady_clock::now() < sync_deadline)
  {
    m_server_job_info_io->JobStateUpdated(sentinel_state);
    updates_flow = m_client_io.WaitForJobState(sentinel_state, 0.05);
  }
  EXPECT_TRUE(updates_flow);
}

WSUserInputE2ETest::~WSUserInputE2ETest()
{
  m_client_job.release();
  m_main_server.release();
  m_server_job_info_io.release();
  m_server_factory.release();
}
