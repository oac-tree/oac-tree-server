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

#include <sup/auto-server/client_reply_delegator.h>

#include <sup/sequencer/user_input_reply.h>

#include <gtest/gtest.h>

#include <cmath>
#include <functional>

using namespace sup::auto_server;
using namespace std::placeholders;

class ClientReplyDelegatorTest : public ::testing::Test
{
protected:
  ClientReplyDelegatorTest();
  virtual ~ClientReplyDelegatorTest() = default;

  void SetTimeout(double timeout_sec);

  ClientReplyDelegator::ReplyFunction GetReplyFunction();
  ClientReplyDelegator::InterruptFunction GetInterruptFunction();

  void Reply(sup::dto::uint64 id, const UserInputReply& reply);
  void Interrupt(sup::dto::uint64 id);

  bool WaitForReply(const UserInputReply& reply, double seconds) const;
  bool WaitForId(sup::dto::uint64 id, double seconds) const;

private:
  bool Wait() const;
  double m_timeout_sec;
  sup::dto::uint64 m_id;
  UserInputReply m_reply;
  mutable std::mutex m_mtx;
  mutable std::condition_variable m_cv;
};

TEST_F(ClientReplyDelegatorTest, NoTimeout)
{
  ClientReplyDelegator delegator{GetReplyFunction(), GetInterruptFunction()};
  sup::dto::uint64 id{42u};
  auto reply = sup::sequencer::CreateUserChoiceReply(true, 5);
  delegator.QueueReply(id, reply);
  EXPECT_TRUE(WaitForReply(reply, 1.0));
  EXPECT_TRUE(WaitForId(0, 1.0));
}

TEST_F(ClientReplyDelegatorTest, WaitForSecond)
{
  ClientReplyDelegator delegator{GetReplyFunction(), GetInterruptFunction()};
  sup::dto::uint64 id{42u};
  auto reply_1 = sup::sequencer::CreateUserChoiceReply(true, 1);
  auto reply_2 = sup::sequencer::CreateUserChoiceReply(true, 2);
  auto reply_3 = sup::sequencer::CreateUserChoiceReply(true, 3);
  SetTimeout(0.2);
  delegator.QueueReply(id, reply_1);
  delegator.QueueReply(id + 1, reply_2);
  delegator.QueueReply(id + 2, reply_3);
  EXPECT_TRUE(WaitForId(id, 1.0));
  EXPECT_TRUE(WaitForReply(reply_1, 1.0));
  EXPECT_TRUE(WaitForId(id + 1, 1.0));
  delegator.InterruptAll();
  EXPECT_FALSE(WaitForReply(reply_2, 0.5));
}

TEST_F(ClientReplyDelegatorTest, InterruptOne)
{
  ClientReplyDelegator delegator{GetReplyFunction(), GetInterruptFunction()};
  sup::dto::uint64 id{42u};
  SetTimeout(1.0);
  auto reply = sup::sequencer::CreateUserChoiceReply(true, 5);
  delegator.QueueReply(id, reply);
  EXPECT_TRUE(WaitForId(id, 1.0));
  delegator.InterruptAll();
  EXPECT_TRUE(WaitForId(0, 1.0));
  EXPECT_TRUE(WaitForReply(sup::sequencer::kInvalidUserInputReply, 1.0));
}

TEST_F(ClientReplyDelegatorTest, InterruptMultiple)
{
  ClientReplyDelegator delegator{GetReplyFunction(), GetInterruptFunction()};
  sup::dto::uint64 id{42u};
  SetTimeout(1.0);
  auto reply = sup::sequencer::CreateUserChoiceReply(true, 5);
  delegator.QueueReply(id, reply);
  delegator.QueueReply(id + 1, reply);
  delegator.QueueReply(id + 2, reply);
  EXPECT_TRUE(WaitForId(id, 1.0));
  delegator.InterruptAll();
  EXPECT_TRUE(WaitForId(0, 1.0));
  EXPECT_TRUE(WaitForReply(sup::sequencer::kInvalidUserInputReply, 1.0));
}

ClientReplyDelegatorTest::ClientReplyDelegatorTest()
  : m_timeout_sec{0.0}
  , m_id{0}
  , m_reply{sup::sequencer::kInvalidUserInputReply}
  , m_mtx{}
  , m_cv{}
{}

void ClientReplyDelegatorTest::SetTimeout(double timeout_sec)
{
  m_timeout_sec = timeout_sec;
}

ClientReplyDelegator::ReplyFunction ClientReplyDelegatorTest::GetReplyFunction()
{
  return std::bind(&ClientReplyDelegatorTest::Reply, this, _1, _2);
}

ClientReplyDelegator::InterruptFunction ClientReplyDelegatorTest::GetInterruptFunction()
{
  return std::bind(&ClientReplyDelegatorTest::Interrupt, this, _1);
}

void ClientReplyDelegatorTest::Reply(sup::dto::uint64 id, const UserInputReply& reply)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_id = id;
  }
  m_cv.notify_one();
  if (Wait())
  {
    return;
  }
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_id = 0;
    m_reply = reply;
  }
  m_cv.notify_one();
}

void ClientReplyDelegatorTest::Interrupt(sup::dto::uint64 id)
{
  (void)id;
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_id = 0;
  }
  m_cv.notify_one();
}

bool ClientReplyDelegatorTest::WaitForReply(const UserInputReply& reply, double seconds) const
{
  auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
  std::unique_lock<std::mutex> lk{m_mtx};
  auto pred = [this, reply](){
    return m_reply == reply;
  };
  return m_cv.wait_for(lk, duration, pred);
}

bool ClientReplyDelegatorTest::WaitForId(sup::dto::uint64 id, double seconds) const
{
  auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
  std::unique_lock<std::mutex> lk{m_mtx};
  auto pred = [this, id](){
    return m_id == id;
  };
  return m_cv.wait_for(lk, duration, pred);
}

bool ClientReplyDelegatorTest::Wait() const
{
  auto duration = std::chrono::nanoseconds(std::lround(m_timeout_sec * 1e9));
  std::unique_lock<std::mutex> lk{m_mtx};
  auto pred = [this](){
    return m_id == 0;
  };
  return m_cv.wait_for(lk, duration, pred);
}