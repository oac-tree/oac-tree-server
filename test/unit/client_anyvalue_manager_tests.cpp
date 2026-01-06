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

#include "unit_test_helper.h"

#include <sup/oac-tree-server/client_anyvalue_manager.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree_server;

using ::testing::_;
using ::testing::InSequence;

class ClientAnyValueManagerTests : public ::testing::Test
{
protected:
  ClientAnyValueManagerTests() = default;
  virtual ~ClientAnyValueManagerTests() = default;

  using StrictMockJobInfoIO = ::testing::StrictMock<UnitTestHelper::MockJobInfoIO>;
  StrictMockJobInfoIO m_test_job_info_io;
};

TEST_F(ClientAnyValueManagerTests, Construction)
{
  ClientAnyValueManager client_av_mgr{m_test_job_info_io};
  {
    // Set Expectations on mock IJobInfoIO calls
    {
      InSequence seq;
      EXPECT_CALL(m_test_job_info_io, JobStateUpdated(sup::oac_tree::JobState::kInitial));
      EXPECT_CALL(m_test_job_info_io, JobStateUpdated(sup::oac_tree::JobState::kRunning));
    }
    // Add only job state anyvalue
    IAnyValueIO::NameAnyValueSet value_set;
    std::string val_name = "prefix:" + kJobStateId;
    value_set.emplace_back(val_name, kJobStateAnyValue);
    client_av_mgr.AddAnyValues(value_set);

    // Update job state anyvalue
    auto new_job_state = kJobStateAnyValue;
    new_job_state[kJobStateField] = static_cast<sup::dto::uint32>(sup::oac_tree::JobState::kRunning);
    client_av_mgr.UpdateAnyValue(val_name, new_job_state);
  }
}
