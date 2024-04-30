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

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/auto-server/epics/epics_job_pv_server.h>
#include <sup/auto-server/epics/epics_instruction_pv_handler.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/epics/pv_access_client_pv.h>
#include <sup/sequencer/application_utils.h>
#include <sup/sequencer/instruction.h>

#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <map>
#include <mutex>

using namespace sup::auto_server;
using namespace sup::sequencer;

// Using different prefixes speeds up the units tests:
const std::string kPrefix1 = "SUP-AUTO-INSTR_TEST1:";
const std::string kPrefix2 = "SUP-AUTO-INSTR_TEST2:";

class EPICSInstructionPVHandlerTest : public ::testing::Test
{
protected:
  EPICSInstructionPVHandlerTest() = default;
  virtual ~EPICSInstructionPVHandlerTest() = default;

  bool WaitForInstrTree(const sup::dto::AnyValue& val, double seconds = 1.0)
  {
    auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
    std::unique_lock<std::mutex> lk{m_mtx};
    return m_cv.wait_for(lk, duration, [this, val](){
      return m_instr_tree == val;
    });
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

  sup::dto::AnyValue m_instr_tree;
  std::mutex m_mtx;
  std::condition_variable m_cv;
};

TEST_F(EPICSInstructionPVHandlerTest, Construction)
{
  // Test construction/destruction of EPICSInstructionPVHandler
  auto instr_tree = UnitTestHelper::CreateTestInstructionTreeAnyValue();
  EPICSInstructionPVHandler pv_handler{kPrefix1, instr_tree};
  EXPECT_NO_THROW(pv_handler.UpdateInstructionTree(instr_tree));
}

TEST_F(EPICSInstructionPVHandlerTest, UpdateInstructionTree)
{
  // Test construction/destruction of EPICSInstructionPVHandler
  auto instr_tree = UnitTestHelper::CreateTestInstructionTreeAnyValue();
  EPICSInstructionPVHandler pv_handler{kPrefix2, instr_tree};

  // Construct client PV for monitoring
  auto pv_callback = [this](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    UpdateInstrTree(val);
  };
  sup::epics::PvAccessClientPV instr_tree_pv{GetInstructionTreePVName(kPrefix2), pv_callback};
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
