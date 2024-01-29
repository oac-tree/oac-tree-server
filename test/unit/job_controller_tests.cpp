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

#include <sup/auto-server/base/job_controller.h>

#include <sup/sequencer/sequence_parser.h>
#include <sup/sequencer/user_interface.h>

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

const std::string kTestExternalProcedureBody{R"(
  <ParallelSequence>
    <Wait timeout="1.0"/>
    <Wait timeout="1.0"/>
  </ParallelSequence>
  <Workspace/>
)"};

using namespace sup::sequencer;
using namespace sup::auto_server;

class JobControllerTest : public ::testing::Test
{
protected:
  JobControllerTest() = default;
  virtual ~JobControllerTest() = default;

  void OnStateChange(JobState state)
  {
    {
      std::lock_guard<std::mutex> lk{m_mtx};
      m_state = state;
    }
    m_cv.notify_one();
  }

  bool WaitForState(JobState state, int seconds = 1)
  {
    std::unique_lock<std::mutex> lk{m_mtx};
    return m_cv.wait_for(lk, std::chrono::seconds(seconds), [this, state](){
      return m_state == state;
    });
  }

  JobState m_state;
  std::mutex m_mtx;
  std::condition_variable m_cv;
};

TEST_F(JobControllerTest, Constructed)
{
  DefaultUserInterface ui;
  auto proc = ParseProcedureString(
    sup::UnitTestHelper::CreateProcedureString(kTestExternalProcedureBody));
  auto cb = [this](JobState state){
    OnStateChange(state);
  };
  JobController controller{*proc, ui, cb};
  EXPECT_TRUE(WaitForState(JobState::kInitial));
  controller.Start();
  EXPECT_TRUE(WaitForState(JobState::kRunning));
  EXPECT_TRUE(WaitForState(JobState::kSucceeded, 2));
}
