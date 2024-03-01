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
#include <sup/protocol/base64_variable_codec.h>
#include <sup/sequencer/application_utils.h>
#include <sup/sequencer/instruction.h>
#include <sup/sequencer/job_controller.h>
#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <map>
#include <mutex>

using namespace sup::auto_server;
using namespace sup::sequencer;

// Using different prefixes speeds up the units tests:
const std::string kPrefix1 = "SUP-AUTO-TEST1:";
const std::string kPrefix2 = "SUP-AUTO-TEST2:";
const std::string kPrefix3 = "SUP-AUTO-TEST3:";
const std::string kPrefix4 = "SUP-AUTO-TEST4:";

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

  bool WaitForVariableValue(const std::string& var_name, const sup::dto::AnyValue& val,
                            double seconds = 1.0)
  {
    auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
    std::unique_lock<std::mutex> lk{m_mtx};
    return m_cv.wait_for(lk, duration, [this, var_name, val](){
      auto iter = m_variables.find(var_name);
      if (iter == m_variables.end())
      {
        return false;
      }
      return iter->second == val;
    });
  }

  void UpdateJobState(const sup::epics::PvAccessClientPV::ExtendedValue& val)
  {
    if (val.connected && !sup::dto::IsEmptyValue(val.value))
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_jobstate = val.value;
    }
    m_cv.notify_one();
  }

  void UpdateInstrTree(const sup::epics::PvAccessClientPV::ExtendedValue& val)
  {
    if (val.connected && !sup::dto::IsEmptyValue(val.value))
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_instr_tree = val.value;
    }
    m_cv.notify_one();
  }

  void UpdateVariable(const std::string& var_name, const sup::epics::PvAccessClientPV::ExtendedValue& val)
  {
    if (val.connected && !sup::dto::IsEmptyValue(val.value))
    {
      auto decoded = sup::protocol::Base64VariableCodec::Decode(val.value);
      if (decoded.first)
      {
        std::lock_guard<std::mutex> lk{m_mtx};
        m_variables[var_name] = decoded.second;
      }
    }
    m_cv.notify_one();
  }

  sup::dto::AnyValue m_jobstate;
  sup::dto::AnyValue m_instr_tree;
  std::map<std::string, sup::dto::AnyValue> m_variables;
  std::mutex m_mtx;
  std::condition_variable m_cv;
};

TEST_F(EPICSPVHandlerTest, Construction)
{
  // Test construction/destruction of EPICSPVHandler
  auto instr_tree = UnitTestHelper::CreateTestInstructionTreeAnyValue();
  JobPVInfo job_pv_info{kPrefix1, instr_tree, 0};
  EPICSPVHandler pv_handler{job_pv_info};
  EXPECT_NO_THROW(pv_handler.UpdateInstructionTree(instr_tree));
}

TEST_F(EPICSPVHandlerTest, UpdateJobState)
{
  // Test construction/destruction of EPICSPVHandler
  auto instr_tree = UnitTestHelper::CreateTestInstructionTreeAnyValue();
  JobPVInfo job_pv_info{kPrefix2, instr_tree, 0};
  EPICSPVHandler pv_handler{job_pv_info};

  // Construct client PV for monitoring
  auto pv_callback = [this](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    UpdateJobState(val);
  };
  sup::epics::PvAccessClientPV job_state_pv{GetJobStatePVName(kPrefix2), pv_callback};
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
  JobPVInfo job_pv_info{kPrefix3, instr_tree, 10};
  EPICSPVHandler pv_handler{job_pv_info};

  // Construct client PV for monitoring
  auto pv_callback = [this](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    UpdateInstrTree(val);
  };
  sup::epics::PvAccessClientPV instr_tree_pv{GetInstructionTreePVName(kPrefix3), pv_callback};
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

TEST_F(EPICSPVHandlerTest, UseJobPVServer)
{
  // Construct procedure
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kShortSequenceBody);
  auto proc = ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());

  // Construct ui and monitor
  auto root_instr = proc->RootInstruction();
  ASSERT_NE(root_instr, nullptr);
  EPICSJobPVServer pv_server{kPrefix4, *proc};
  ServerUserInterface ui{pv_server};
  ServerJobStateMonitor monitor{pv_server};

    // Construct client PVs for monitoring
  auto pv_job_callback = [this](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    UpdateJobState(val);
  };
  sup::epics::PvAccessClientPV job_state_pv{GetJobStatePVName(kPrefix4), pv_job_callback};
  EXPECT_TRUE(job_state_pv.WaitForValidValue(1.0));
  auto pv_instr_callback = [this](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    UpdateInstrTree(val);
  };
  sup::epics::PvAccessClientPV instr_tree_pv{GetInstructionTreePVName(kPrefix4), pv_instr_callback};
  EXPECT_TRUE(instr_tree_pv.WaitForValidValue(1.0));

  // Construct JobController and run procedure
  JobController controller{*proc, ui, monitor};
  auto instr_tree = UnitTestHelper::CreateTestInstructionTreeAnyValue();
  EXPECT_TRUE(WaitForJobstate(JobState::kInitial));
  EXPECT_TRUE(WaitForInstrTree(instr_tree));
  controller.SetBreakpoint(root_instr);
  instr_tree[kBreakpointField] = true;
  EXPECT_TRUE(WaitForInstrTree(instr_tree));
  controller.Start();
  EXPECT_TRUE(WaitForJobstate(JobState::kPaused));
  controller.RemoveBreakpoint(root_instr);
  instr_tree[kBreakpointField] = false;
  EXPECT_TRUE(WaitForInstrTree(instr_tree));
  controller.Start();
  EXPECT_TRUE(WaitForJobstate(JobState::kSucceeded));
}

TEST_F(EPICSPVHandlerTest, UpdateVariable)
{
  // Test construction/destruction of EPICSPVHandler
  auto instr_tree = UnitTestHelper::CreateTestInstructionTreeAnyValue();
  JobPVInfo job_pv_info{kPrefix2, instr_tree, 1};
  EPICSPVHandler pv_handler{job_pv_info};

  // Construct client PV for monitoring
  const std::string var_name = "var1";
  auto pv_callback = [this, var_name](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    UpdateVariable(var_name, val);
  };
  sup::epics::PvAccessClientPV variable_pv{GetVariablePVName(kPrefix2, 0), pv_callback};
  EXPECT_TRUE(variable_pv.WaitForValidValue(1.0));
  auto var_encoded = variable_pv.GetValue();
  EXPECT_EQ(var_encoded.GetType(), kVariableAnyValue.GetType());

  // Update variable
  sup::dto::AnyValue var_update = {{
    { "setpoint", 42.0 },
    { "enabled", false }
  }};
  auto var_info = EncodeVariableInfo(var_update, true);
  pv_handler.UpdateVariable(0, var_info);
  sup::dto::AnyValue payload = {{
    { kVariableValueField, var_update },
    { kVariableConnectedField, true }
  }};
  EXPECT_TRUE(WaitForVariableValue(var_name, payload));

  // Update variable to empty and not connected
  sup::dto::AnyValue var_update_empty{};
  var_info = EncodeVariableInfo(var_update_empty, false);
  pv_handler.UpdateVariable(0, var_info);
  payload[kVariableValueField] = var_update_empty;
  payload[kVariableConnectedField] = false;
  EXPECT_TRUE(WaitForVariableValue(var_name, payload));
}
