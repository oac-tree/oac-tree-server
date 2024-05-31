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

#include "unit_test_helper.h"

#include <sup/auto-server/job.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/epics/pv_access_client_pv.h>
#include <sup/sequencer/sequence_parser.h>

#include <chrono>
#include <cmath>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class JobTest : public ::testing::Test
{
protected:
  JobTest() = default;
  virtual ~JobTest() = default;

  void OnUpdateValue(const sup::dto::AnyValue& value)
  {
    std::lock_guard<std::mutex> lk(m_mtx);
    m_value_cache = value;
    m_cv.notify_one();
  }

  bool WaitForValue(const sup::dto::AnyValue& value, double seconds)
  {
    auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
    std::unique_lock<std::mutex> lk{m_mtx};
    auto pred = [this, value](){
      return m_value_cache == value;
    };
    return m_cv.wait_for(lk, duration, pred);
  }

  sup::dto::AnyValue m_value_cache;
  std::mutex m_mtx;
  std::condition_variable m_cv;
};

TEST_F(JobTest, Constructed)
{
  const std::string prefix = "JobTest:Constructed:";
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kLongWaitProcedureBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);

  // Create Job and wait for initial state
  Job job{prefix, std::move(proc)};

  auto pv_callback = [this](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    if(val.connected)
    {
      OnUpdateValue(val.value);
    }
  };
  const auto state_channel = prefix + kJobStateId;
  sup::epics::PvAccessClientPV state_pv{state_channel, pv_callback};
  EXPECT_TRUE(state_pv.WaitForValidValue(3.0));
  auto state = state_pv.GetValue();
  EXPECT_EQ(state, kJobStateAnyValue);

  // Start job and wait for running state
  job.Start();
  auto running_state = kJobStateAnyValue;
  running_state[kJobStateField] = static_cast<sup::dto::uint32>(sup::sequencer::JobState::kRunning);
  EXPECT_TRUE(WaitForValue(running_state, 1.0));

  // Halt job and wait for failed state
  job.Halt();
  auto failed_state = kJobStateAnyValue;
  failed_state[kJobStateField] = static_cast<sup::dto::uint32>(sup::sequencer::JobState::kFailed);
  EXPECT_TRUE(WaitForValue(failed_state, 1.0));
}

TEST_F(JobTest, MoveConstructed)
{
  const std::string prefix = "JobTest:MoveConstructed:";
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kLongWaitProcedureBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);

  // Create Job and wait for initial state
  Job tmp_job{prefix, std::move(proc)};
  Job job{std::move(tmp_job)};

  auto pv_callback = [this](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    if(val.connected)
    {
      OnUpdateValue(val.value);
    }
  };
  const auto state_channel = prefix + kJobStateId;
  sup::epics::PvAccessClientPV state_pv{state_channel, pv_callback};
  EXPECT_TRUE(state_pv.WaitForValidValue(3.0));
  auto state = state_pv.GetValue();
  EXPECT_EQ(state, kJobStateAnyValue);

  // Start job and wait for running state
  job.Start();
  auto running_state = kJobStateAnyValue;
  running_state[kJobStateField] = static_cast<sup::dto::uint32>(sup::sequencer::JobState::kRunning);
  EXPECT_TRUE(WaitForValue(running_state, 1.0));

  // Halt job and wait for failed state
  job.Halt();
  auto failed_state = kJobStateAnyValue;
  failed_state[kJobStateField] = static_cast<sup::dto::uint32>(sup::sequencer::JobState::kFailed);
  EXPECT_TRUE(WaitForValue(failed_state, 1.0));
}
