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

#include <sup/auto-server/epics/epics_anyvalue_manager.h>
#include <sup/auto-server/epics_anyvalue_listener.h>

#include <gtest/gtest.h>

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
}

EPICSManagerListenerTest::EPICSManagerListenerTest()
  : m_test_av_manager{}
  , m_epics_listener{m_test_av_manager}
  , m_epics_av_manager{}
{}