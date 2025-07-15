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
 * Copyright (c) : 2010-2025 ITER Organization,
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

#include <sup/oac-tree-server/automation_client_stack.h>
#include <sup/oac-tree-server/exceptions.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>
#include <sup/oac-tree-server/automation_protocol_client.h>
#include <sup/oac-tree-server/control_protocol_server.h>
#include <sup/oac-tree-server/info_protocol_server.h>

#include <sup/oac-tree/sequence_parser.h>

#include <gtest/gtest.h>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Exactly;
using ::testing::Throw;
using ::testing::Return;
using ::testing::SetArgReferee;

using namespace sup::oac_tree_server;

const std::string kTestPrefix = "AutomationClientTests";

class AutomationClientTests : public ::testing::Test
{
protected:
  AutomationClientTests() = default;
  virtual ~AutomationClientTests() = default;

  UnitTestHelper::TestAnyValueManagerRegistry m_test_av_mgr_registry;
};

TEST_F(AutomationClientTests, AutomationClientStackFailure)
{
  EXPECT_THROW({AutomationClientStack client_stack(nullptr, nullptr);}, InvalidOperationException);
}


class AutomationProtocolClientFailureTest : public ::testing::Test {
protected:
  AutomationProtocolClientFailureTest()
      : m_info_server(m_mock_job_manager),
        m_control_server(m_mock_job_manager),
        m_client(m_info_server, m_control_server) {}

  ::testing::StrictMock<UnitTestHelper::MockJobManager> m_mock_job_manager;
  InfoProtocolServer m_info_server;
  ControlProtocolServer m_control_server;
  AutomationProtocolClient m_client;
};

TEST_F(AutomationProtocolClientFailureTest, UnknownJob) {
  const sup::dto::uint32 invalid_job_id = 999;

  // Mock GetNumberOfJobs to return a value greater than invalid_job_id
  EXPECT_CALL(m_mock_job_manager, GetNumberOfJobs()).Times(Exactly(1))
    .WillOnce(Return(1000));

  // Mock GetJobInfo to throw an exception for the invalid job ID
  EXPECT_CALL(m_mock_job_manager, GetJobInfo(invalid_job_id)).Times(Exactly(1))
    .WillOnce(Throw(InvalidOperationException("UnknownJob")));

  // Verify that the client throws the exception when calling GetJobInfo
  EXPECT_THROW(m_client.GetJobInfo(invalid_job_id), InvalidOperationException);
}

TEST_F(AutomationProtocolClientFailureTest, UnexpectedProtocolResult) {
  const sup::dto::uint32 job_id = 0;

  // Mock GetNumberOfJobs to throw an exception for an unexpected protocol result
  EXPECT_CALL(m_mock_job_manager, GetNumberOfJobs()).Times(Exactly(1))
    .WillOnce(Throw(InvalidOperationException("UnexpectedProtocolResult")));

  // Verify that the client throws the exception when calling GetNumberOfJobs
  EXPECT_THROW(m_client.GetNumberOfJobs(), InvalidOperationException);
}
