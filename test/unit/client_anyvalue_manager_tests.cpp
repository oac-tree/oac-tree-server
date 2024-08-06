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

#include <sup/auto-server/client_anyvalue_manager.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class ClientAnyValueManagerTests : public ::testing::Test
{
protected:
  ClientAnyValueManagerTests() = default;
  virtual ~ClientAnyValueManagerTests() = default;

  UnitTestHelper::TestJobInfoIO m_test_job_info_io;
};

TEST_F(ClientAnyValueManagerTests, Construction)
{
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  {
    // Add only job state anyvalue
    IAnyValueManager::NameAnyValueSet value_set;
    std::string val_name = "prefix:" + kJobStateId;
    value_set.emplace_back(val_name, kJobStateAnyValue);
    client_av_mgr.AddAnyValues(value_set);
    auto job_states = m_test_job_info_io.GetJobStates();
    ASSERT_EQ(job_states.size(), 1);
    auto last_job_state = job_states.back();
    EXPECT_EQ(last_job_state, sup::sequencer::JobState::kInitial);

    // Update job state anyvalue
    auto new_job_state = kJobStateAnyValue;
    new_job_state[kJobStateField] = static_cast<sup::dto::uint32>(sup::sequencer::JobState::kRunning);
    client_av_mgr.UpdateAnyValue(val_name, new_job_state);
    job_states = m_test_job_info_io.GetJobStates();
    ASSERT_EQ(job_states.size(), 2);
    last_job_state = job_states.back();
    EXPECT_EQ(last_job_state, sup::sequencer::JobState::kRunning);
  }
}
