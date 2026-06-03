/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : Unit test code
 *
 * Author        : Marcelo Mariano (External)
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

#include <sup/oac-tree-server/base/automation_protocol_client.h>
#include <sup/oac-tree-server/exceptions.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>
#include <sup/oac-tree/exceptions.h>
#include <sup/protocol/function_protocol_pack.h>
#include <sup/protocol/protocol.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Exactly;
using ::testing::Return;
using ::testing::SetArgReferee;

using namespace sup::oac_tree_server;

class MockProtocol : public sup::protocol::Protocol
{
public:
  MOCK_METHOD(sup::protocol::ProtocolResult, Invoke,
              (const sup::dto::AnyValue&, sup::dto::AnyValue&), (override));
  MOCK_METHOD(sup::protocol::ProtocolResult, Service,
              (const sup::dto::AnyValue&, sup::dto::AnyValue&), (override));
};

class AutomationProtocolClientMockTest : public ::testing::Test
{
protected:
  AutomationProtocolClientMockTest() : m_client{m_info_protocol, m_control_protocol} {}

  ::testing::StrictMock<MockProtocol> m_info_protocol;
  ::testing::StrictMock<MockProtocol> m_control_protocol;
  AutomationProtocolClient m_client;
};

/**
 * @brief GetServerPrefix throws when the info protocol returns a non-success result.
 */
TEST_F(AutomationProtocolClientMockTest, GetServerPrefixProtocolError)
{
  EXPECT_CALL(m_info_protocol, Invoke(_, _)).Times(Exactly(1)).WillOnce(Return(NotSupported));
  EXPECT_THROW(m_client.GetServerPrefix(), InvalidOperationException);
}

/**
 * @brief GetServerPrefix throws when the reply does not contain the server-prefix field.
 */
TEST_F(AutomationProtocolClientMockTest, GetServerPrefixMissingField)
{
  EXPECT_CALL(m_info_protocol, Invoke(_, _))
      .Times(Exactly(1))
      .WillOnce(Return(sup::protocol::Success));
  EXPECT_THROW(m_client.GetServerPrefix(), InvalidOperationException);
}

/**
 * @brief GetNumberOfJobs throws when the info protocol returns a non-success result.
 */
TEST_F(AutomationProtocolClientMockTest, GetNumberOfJobsProtocolError)
{
  EXPECT_CALL(m_info_protocol, Invoke(_, _)).Times(Exactly(1)).WillOnce(Return(NotSupported));
  EXPECT_THROW(m_client.GetNumberOfJobs(), InvalidOperationException);
}

/**
 * @brief GetNumberOfJobs throws when the reply does not contain a valid number-of-jobs field.
 */
TEST_F(AutomationProtocolClientMockTest, GetNumberOfJobsBadReply)
{
  EXPECT_CALL(m_info_protocol, Invoke(_, _))
      .Times(Exactly(1))
      .WillOnce(Return(sup::protocol::Success));
  EXPECT_THROW(m_client.GetNumberOfJobs(), InvalidOperationException);
}

/**
 * @brief GetJobInfo throws when the info protocol returns a non-success result.
 */
TEST_F(AutomationProtocolClientMockTest, GetJobInfoProtocolError)
{
  EXPECT_CALL(m_info_protocol, Invoke(_, _)).Times(Exactly(1)).WillOnce(Return(NotSupported));
  EXPECT_THROW(m_client.GetJobInfo(0u), InvalidOperationException);
}

/**
 * @brief GetJobInfo throws when the reply does not contain the job-info field.
 */
TEST_F(AutomationProtocolClientMockTest, GetJobInfoMissingField)
{
  EXPECT_CALL(m_info_protocol, Invoke(_, _))
      .Times(Exactly(1))
      .WillOnce(Return(sup::protocol::Success));
  EXPECT_THROW(m_client.GetJobInfo(0u), InvalidOperationException);
}

/**
 * @brief GetJobInfo throws when the job-info AnyValue cannot be converted to a JobInfo.
 */
TEST_F(AutomationProtocolClientMockTest, GetJobInfoInvalidJobInfoValue)
{
  sup::dto::AnyValue mock_output;
  sup::dto::AnyValue invalid_job_info{sup::dto::StringType, std::string{"invalid"}};
  sup::protocol::FunctionProtocolPack(mock_output, kJobInfoFieldName, invalid_job_info);
  EXPECT_CALL(m_info_protocol, Invoke(_, _))
      .Times(Exactly(1))
      .WillOnce(DoAll(SetArgReferee<1>(mock_output), Return(sup::protocol::Success)));
  EXPECT_THROW(m_client.GetJobInfo(0u), sup::oac_tree::InvalidOperationException);
}

/**
 * @brief EditBreakpoint throws when the control protocol returns a non-success result.
 */
TEST_F(AutomationProtocolClientMockTest, EditBreakpointProtocolError)
{
  EXPECT_CALL(m_control_protocol, Invoke(_, _)).Times(Exactly(1)).WillOnce(Return(NotSupported));
  EXPECT_THROW(m_client.EditBreakpoint(0u, 0u, true), InvalidOperationException);
}

/**
 * @brief SendJobCommand throws when the control protocol returns a non-success result.
 */
TEST_F(AutomationProtocolClientMockTest, SendJobCommandProtocolError)
{
  EXPECT_CALL(m_control_protocol, Invoke(_, _)).Times(Exactly(1)).WillOnce(Return(NotSupported));
  EXPECT_THROW(m_client.SendJobCommand(0u, sup::oac_tree::JobCommand::kStart),
               InvalidOperationException);
}
