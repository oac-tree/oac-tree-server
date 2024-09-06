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
#include <sup/auto-server/epics/epics_anyvalue_manager.h>
#include <sup/auto-server/epics_anyvalue_listener.h>
#include <sup/auto-server/input_protocol_client.h>

#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/pv_access_rpc_client.h>
#include <sup/protocol/protocol_rpc.h>

#include <gtest/gtest.h>

#include <thread>
#include <chrono>

using namespace sup::auto_server;

namespace
{
const sup::dto::AnyValue scalar = {{
  { "value", {sup::dto::SignedInteger32Type, 0}}
}};

IAnyValueManager::NameAnyValueSet value_set_1 = {
  { "val0", scalar},
  { "val1", scalar}
};
}  // unnamed namespace

class EPICSManagerListenerTest : public ::testing::Test
{
protected:
  EPICSManagerListenerTest();

  virtual ~EPICSManagerListenerTest() = default;

  UnitTestHelper::TestAnyValueManager m_test_av_manager;
  EPICSAnyValueListener m_epics_listener;
  EPICSAnyValueManager m_epics_av_manager;
};

TEST_F(EPICSManagerListenerTest, AddValuesAndUpdate)
{
  // Add values and wait for first value (must be immediate)
  ASSERT_TRUE(m_epics_av_manager.AddAnyValues(value_set_1));
  ASSERT_TRUE(m_epics_listener.AddAnyValueMonitors(value_set_1));
  EXPECT_TRUE(m_test_av_manager.WaitForValue("val0", scalar, 0.0));
  EXPECT_TRUE(m_test_av_manager.WaitForValue("val1", scalar, 0.0));

  // Update values on manager side and wait for the updates to arrive at the listener
  const sup::dto::AnyValue update = {{
    { "value", {sup::dto::SignedInteger32Type, 42}}
  }};
  EXPECT_TRUE(m_epics_av_manager.UpdateAnyValue("val0", update));
  EXPECT_TRUE(m_test_av_manager.WaitForValue("val0", update, 5.0));
  EXPECT_TRUE(m_epics_av_manager.UpdateAnyValue("val1", update));
  EXPECT_TRUE(m_test_av_manager.WaitForValue("val1", update, 5.0));
  EXPECT_EQ(m_test_av_manager.GetNbrInputRequests(), 0);

  // Trying to update of wait for a value that doesn't exist fails
  EXPECT_FALSE(m_epics_av_manager.UpdateAnyValue("does_not_exist", update));
  EXPECT_FALSE(m_test_av_manager.WaitForValue("does_not_exist", update, 0.1));
}

TEST_F(EPICSManagerListenerTest, ProtocolInformation)
{
  // Add input server
  const std::string input_server_name = "TestInputServer01";
  ASSERT_TRUE(m_epics_av_manager.AddInputServer(input_server_name));

  // Query application protocol information
  auto client_protocol =
    sup::epics::CreateEPICSRPCClientStack(sup::epics::GetDefaultRPCClientConfig(input_server_name));
  auto protocol_info = sup::protocol::utils::GetApplicationProtocolInfo(*client_protocol);
  EXPECT_EQ(protocol_info.m_application_type, kAutomationInputRequestServerType);
  EXPECT_EQ(protocol_info.m_application_version, kAutomationInputRequestServerVersion);
}

TEST_F(EPICSManagerListenerTest, GetUserInput)
{
  // Add input servers
  const std::string input_server_name = "TestInputServer03";
  ASSERT_TRUE(m_epics_av_manager.AddInputServer(input_server_name));

  ASSERT_TRUE(m_epics_listener.AddInputClient(input_server_name));

  auto input_server_pv = GetInputRequestPVName(input_server_name);
  ASSERT_TRUE(m_test_av_manager.HasAnyValue(input_server_pv));
  EXPECT_EQ(m_test_av_manager.GetNbrInputRequests(), 0);


  // Get user input over the network
  sup::dto::AnyValue user_reply{ sup::dto::UnsignedInteger64Type, 42u };
  m_test_av_manager.SetUserInput(user_reply);
  sup::dto::AnyValue empty{};
  auto input_request = CreateUserValueRequest(empty, "Provide a value");
  auto reply_received = m_epics_av_manager.GetUserInput(input_server_name, input_request);
  EXPECT_EQ(reply_received, user_reply);
  EXPECT_EQ(m_test_av_manager.GetNbrInputRequests(), 1);
}

EPICSManagerListenerTest::EPICSManagerListenerTest()
  : m_test_av_manager{}
  , m_epics_listener{m_test_av_manager}
  , m_epics_av_manager{}
{}