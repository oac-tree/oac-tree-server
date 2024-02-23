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

#include <sup/auto-server/server_job_state_monitor.h>
#include <sup/auto-server/server_ui.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/auto-server/epics/epics_job_pv_server.h>
#include <sup/auto-server/epics/epics_pv_handler.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/epics/pv_access_client_pv.h>
#include <sup/sequencer/application_utils.h>
#include <sup/sequencer/instruction.h>
#include <sup/sequencer/job_controller.h>
#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <mutex>

using namespace sup::auto_server;
using namespace sup::sequencer;

const std::string kPrefix = "SUP-AUTO-TEST:";

class EPICSPVHandlerTest : public ::testing::Test
{
protected:
  EPICSPVHandlerTest()
    : m_jobstate{kJobStateAnyValue}
    , m_instr_tree{}
  {}
  virtual ~EPICSPVHandlerTest() = default;

  bool WaitForJobstate(JobState state, double seconds = 1.0)
  {
    auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
    std::unique_lock<std::mutex> lk{m_mtx};
    return m_cv.wait_for(lk, duration, [this, state](){
      return m_jobstate[kJobStateField] == static_cast<sup::dto::uint32>(state);
    });
  }

  bool WaitForInstrTree(const sup::dto::AnyValue& val, double seconds = 1.0)
  {
    auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
    std::unique_lock<std::mutex> lk{m_mtx};
    return m_cv.wait_for(lk, duration, [this, val](){
      return m_instr_tree == val;
    });
  }

  void UpdateJobState(const sup::epics::PvAccessClientPV::ExtendedValue& val)
  {
    if (val.connected)
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_jobstate = val.value;
    }
    m_cv.notify_one();
  }

  void UpdateInstrTree(const sup::epics::PvAccessClientPV::ExtendedValue& val)
  {
    if (val.connected)
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_instr_tree = val.value;
    }
    m_cv.notify_one();
  }

  sup::dto::AnyValue m_jobstate;
  sup::dto::AnyValue m_instr_tree;
  std::mutex m_mtx;
  std::condition_variable m_cv;
};

TEST_F(EPICSPVHandlerTest, Construction)
{
  // Test construction/destruction of EPICSPVHandler
  auto instr_tree = UnitTestHelper::CreateTestInstructionTreeAnyValue();
  EPICSPVHandler pv_handler{kPrefix, instr_tree};
  EXPECT_NO_THROW(pv_handler.UpdateInstructionTree(instr_tree));
}

TEST_F(EPICSPVHandlerTest, UpdateJobState)
{
  // Test construction/destruction of EPICSPVHandler
  auto instr_tree = UnitTestHelper::CreateTestInstructionTreeAnyValue();
  EPICSPVHandler pv_handler{kPrefix, instr_tree};

  // Construct client PV for monitoring
  auto pv_callback = [this](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    UpdateJobState(val);
  };
  sup::epics::PvAccessClientPV job_state_pv{GetJobStatePVName(kPrefix), pv_callback};
  EXPECT_TRUE(job_state_pv.WaitForValidValue(1.0));
  auto job_state = job_state_pv.GetValue();
  EXPECT_EQ(job_state, kJobStateAnyValue);

  // Update jobstate to kRunning
  sup::dto::AnyValue job_update = kJobStateAnyValue;
  job_update[kJobStateField] = static_cast<sup::dto::uint32>(JobState::kRunning);
  pv_handler.UpdateJobState(job_update);
  EXPECT_TRUE(WaitForJobstate(JobState::kRunning));

  // Update jobstate to kSucceeded
  job_update[kJobStateField] = static_cast<sup::dto::uint32>(JobState::kSucceeded);
  pv_handler.UpdateJobState(job_update);
  EXPECT_TRUE(WaitForJobstate(JobState::kSucceeded));
}

TEST_F(EPICSPVHandlerTest, UpdateInstructionTree)
{
  // Test construction/destruction of EPICSPVHandler
  auto instr_tree = UnitTestHelper::CreateTestInstructionTreeAnyValue();
  EPICSPVHandler pv_handler{kPrefix, instr_tree};

  // Construct client PV for monitoring
  auto pv_callback = [this](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    UpdateInstrTree(val);
  };
  sup::epics::PvAccessClientPV instr_tree_pv{GetInstructionTreePVName(kPrefix), pv_callback};
  EXPECT_TRUE(instr_tree_pv.WaitForValidValue(1.0));
  EXPECT_TRUE(WaitForInstrTree(instr_tree));

  // Update root instruction status and breakpoint
  instr_tree[kExecStatusField] =
    static_cast<sup::dto::uint16>(sup::sequencer::ExecutionStatus::NOT_FINISHED);
  instr_tree[kBreakpointField] = true;
  pv_handler.UpdateInstructionTree(instr_tree);
  EXPECT_TRUE(WaitForInstrTree(instr_tree));

  // Update child instruction status and breakpoint
  auto& children = instr_tree[kChildrenField];
  ASSERT_TRUE(children.HasField("Wait1"));
  children["Wait1"][kExecStatusField] =
    static_cast<sup::dto::uint16>(sup::sequencer::ExecutionStatus::FAILURE);
  children["Wait1"][kBreakpointField] = true;
  pv_handler.UpdateInstructionTree(instr_tree);
  EXPECT_TRUE(WaitForInstrTree(instr_tree));
}
