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

#include <sup/oac-tree-server/websocket/ws_client_input_delegator.h>
#include <sup/oac-tree-server/websocket/ws_message.h>

#include <sup/oac-tree/i_job_info_io.h>

#include <sup/dto/anyvalue.h>

#include <gtest/gtest.h>

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <string>
#include <vector>

using namespace sup::oac_tree_server;

namespace
{
// Thread-safe recorder for the reply messages the delegator sends.
class SendRecorder
{
public:
  WSClientInputDelegator::SendFunction Function()
  {
    return [this](const std::string& message) {
      {
        std::lock_guard<std::mutex> lk{m_mtx};
        m_messages.push_back(message);
      }
      m_cv.notify_all();
    };
  }

  std::vector<std::string> WaitFor(std::size_t count)
  {
    std::unique_lock<std::mutex> lk{m_mtx};
    m_cv.wait(lk, [&] { return m_messages.size() >= count; });
    return m_messages;
  }

  std::size_t Size()
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    return m_messages.size();
  }

private:
  std::mutex m_mtx;
  std::condition_variable m_cv;
  std::vector<std::string> m_messages;
};

// IJobInfoIO test double whose GetUserValue/GetUserChoice block (like a real UI dialog) until the
// test provides an answer, the local user cancels, or the prompt is interrupted.
class FakeInputUI : public sup::oac_tree::IJobInfoIO
{
public:
  // Wait until at least `count` prompts have been entered (a counter, not a flag, so a later wait
  // is never satisfied by an earlier prompt).
  void WaitForPrompt(int count)
  {
    std::unique_lock<std::mutex> lk{m_mtx};
    m_cv.wait(lk, [this, count] { return m_prompt_count >= count; });
  }
  void ProvideValue(const sup::dto::AnyValue& value)
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_value = value;
      m_value_ready = true;
    }
    m_cv.notify_all();
  }
  void ProvideChoice(int choice)
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_choice = choice;
      m_choice_ready = true;
    }
    m_cv.notify_all();
  }
  void UserCancel()  // local user dismisses the dialog (not a server-side cancellation)
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_user_cancel = true;
    }
    m_cv.notify_all();
  }
  int ValueCalls()
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    return m_value_calls;
  }
  bool InterruptCalled()
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    return m_interrupt_called;
  }

  bool GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                    const std::string& description) override
  {
    (void)id;
    (void)description;
    std::unique_lock<std::mutex> lk{m_mtx};
    ++m_value_calls;
    m_value_ready = false;
    m_interrupted = false;
    m_user_cancel = false;
    ++m_prompt_count;
    m_cv.notify_all();
    m_cv.wait(lk, [this] { return m_value_ready || m_interrupted || m_user_cancel; });
    if (m_interrupted || m_user_cancel)
    {
      return false;
    }
    value = m_value;
    return true;
  }

  int GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                    const sup::dto::AnyValue& metadata) override
  {
    (void)id;
    (void)options;
    (void)metadata;
    std::unique_lock<std::mutex> lk{m_mtx};
    m_choice_ready = false;
    m_interrupted = false;
    m_user_cancel = false;
    ++m_prompt_count;
    m_cv.notify_all();
    m_cv.wait(lk, [this] { return m_choice_ready || m_interrupted || m_user_cancel; });
    if (m_interrupted || m_user_cancel)
    {
      return -1;
    }
    return m_choice;
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

  // Unused IJobInfoIO methods.
  void InitNumberOfInstructions(sup::dto::uint32) override {}
  void InstructionStateUpdated(sup::dto::uint32, sup::oac_tree::InstructionState) override {}
  void BreakpointInstructionUpdated(sup::dto::uint32) override {}
  void VariableUpdated(sup::dto::uint32, const sup::dto::AnyValue&, bool) override {}
  void JobStateUpdated(sup::oac_tree::JobState) override {}
  void PutValue(const sup::dto::AnyValue&, const std::string&) override {}
  void Message(const std::string&) override {}
  void Log(int, const std::string&) override {}
  void ProcedureTicked() override {}

private:
  std::mutex m_mtx;
  std::condition_variable m_cv;
  int m_prompt_count{0};
  bool m_value_ready{false};
  bool m_choice_ready{false};
  bool m_interrupted{false};
  bool m_user_cancel{false};
  bool m_interrupt_called{false};
  int m_value_calls{0};
  sup::dto::AnyValue m_value{};
  int m_choice{-1};
};
}  // namespace

TEST(WSClientInputDelegatorTest, UserValueRequestIsRepliedTo)
{
  FakeInputUI ui;
  SendRecorder recorder;
  WSClientInputDelegator delegator{ui};

  const sup::dto::uint64 id{42u};
  const sup::dto::AnyValue def{sup::dto::SignedInteger32Type, 0};
  delegator.QueueUserValueRequest(id, def, "description", recorder.Function());

  ui.WaitForPrompt(1);
  const sup::dto::AnyValue provided{sup::dto::SignedInteger32Type, 7};
  ui.ProvideValue(provided);

  auto messages = recorder.WaitFor(1);
  ASSERT_EQ(messages.size(), 1u);
  EXPECT_EQ(messages[0], CreateGetUserValueReplyMessage(id, true, provided));
}

TEST(WSClientInputDelegatorTest, UserChoiceRequestIsRepliedTo)
{
  FakeInputUI ui;
  SendRecorder recorder;
  WSClientInputDelegator delegator{ui};

  const sup::dto::uint64 id{1u};
  delegator.QueueUserChoiceRequest(id, {"yes", "no"}, sup::dto::AnyValue{}, recorder.Function());

  ui.WaitForPrompt(1);
  ui.ProvideChoice(1);

  auto messages = recorder.WaitFor(1);
  ASSERT_EQ(messages.size(), 1u);
  EXPECT_EQ(messages[0], CreateGetUserChoiceReplyMessage(id, 1));
}

TEST(WSClientInputDelegatorTest, LocalUserCancelSendsNegativeReply)
{
  FakeInputUI ui;
  SendRecorder recorder;
  WSClientInputDelegator delegator{ui};

  const sup::dto::uint64 id{5u};
  const sup::dto::AnyValue def{sup::dto::SignedInteger32Type, 0};
  delegator.QueueUserValueRequest(id, def, "description", recorder.Function());

  ui.WaitForPrompt(1);
  ui.UserCancel();

  auto messages = recorder.WaitFor(1);
  ASSERT_EQ(messages.size(), 1u);
  EXPECT_EQ(messages[0], CreateGetUserValueReplyMessage(id, false, def));
}

TEST(WSClientInputDelegatorTest, ResentRequestPromptsOnce)
{
  FakeInputUI ui;
  SendRecorder recorder;
  WSClientInputDelegator delegator{ui};

  const sup::dto::uint64 id{7u};
  const sup::dto::AnyValue def{sup::dto::SignedInteger32Type, 0};
  // Same id twice: the second is a re-publication and must be ignored.
  delegator.QueueUserValueRequest(id, def, "description", recorder.Function());
  delegator.QueueUserValueRequest(id, def, "description", recorder.Function());

  ui.WaitForPrompt(1);
  const sup::dto::AnyValue provided{sup::dto::SignedInteger32Type, 3};
  ui.ProvideValue(provided);

  auto messages = recorder.WaitFor(1);
  EXPECT_EQ(messages.size(), 1u);
  EXPECT_EQ(ui.ValueCalls(), 1);
}

TEST(WSClientInputDelegatorTest, CancelInterruptsAndSuppressesReply)
{
  FakeInputUI ui;
  SendRecorder recorder;
  WSClientInputDelegator delegator{ui};

  const sup::dto::uint64 cancelled_id{7u};
  const sup::dto::AnyValue def{sup::dto::SignedInteger32Type, 0};
  delegator.QueueUserValueRequest(cancelled_id, def, "description", recorder.Function());

  ui.WaitForPrompt(1);
  delegator.Cancel(cancelled_id);
  EXPECT_TRUE(ui.InterruptCalled());

  // Queue a second request and wait for its reply. Because the worker is single-threaded and FIFO,
  // observing the second reply proves the cancelled request did not produce one.
  const sup::dto::uint64 next_id{8u};
  delegator.QueueUserValueRequest(next_id, def, "description", recorder.Function());
  ui.WaitForPrompt(2);
  const sup::dto::AnyValue provided{sup::dto::SignedInteger32Type, 9};
  ui.ProvideValue(provided);

  auto messages = recorder.WaitFor(1);
  ASSERT_EQ(messages.size(), 1u);
  EXPECT_EQ(messages[0], CreateGetUserValueReplyMessage(next_id, true, provided));
}
