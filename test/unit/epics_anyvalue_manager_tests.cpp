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

#include <sup/epics/pv_access_client_pv.h>

#include <chrono>
#include <cmath>

#include <gtest/gtest.h>

using namespace sup::auto_server;

const sup::dto::AnyValue scalar = {{
  { "value", {sup::dto::SignedInteger32Type, 0}}
}};

IAnyValueManager::NameAnyValueSet value_set_1 = {
  { "val0", scalar},
  { "val1", scalar}
};

IAnyValueManager::NameAnyValueSet value_set_2 = {
  { "val2", scalar},
  { "val3", scalar}
};

IAnyValueManager::NameAnyValueSet value_set_3 = {
  { "val1", scalar}
};

class EPICSAnyValueManagerTest : public ::testing::Test
{
protected:
  EPICSAnyValueManagerTest() = default;
  virtual ~EPICSAnyValueManagerTest() = default;

  void OnUpdateValue(const sup::dto::AnyValue& value)
  {
    std::lock_guard<std::mutex> lk(m_mtx);
    m_value_cache = value;
    m_cv.notify_one();
  }

  bool WaitForValue(const sup::dto::AnyValue& value, double seconds)
  {
    auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
    std::unique_lock<std::mutex> lk{m_mtx};
    auto pred = [this, value](){
      return m_value_cache == value;
    };
    return m_cv.wait_for(lk, duration, pred);
  }

  EPICSAnyValueManager m_epics_av_manager;
  sup::dto::AnyValue m_value_cache;
  std::mutex m_mtx;
  std::condition_variable m_cv;
};

TEST_F(EPICSAnyValueManagerTest, ServeAndUpdate)
{
  // Serve value set and construct client PV for monitoring
  ASSERT_TRUE(m_epics_av_manager.AddAnyValues(value_set_1));
  auto pv_callback = [this](const sup::epics::PvAccessClientPV::ExtendedValue& val) {
    if(val.connected)
    {
      OnUpdateValue(val.value);
    }
  };
  sup::epics::PvAccessClientPV val0_pv{"val0", pv_callback};
  EXPECT_TRUE(val0_pv.WaitForValidValue(1.0));
  auto val0 = val0_pv.GetValue();
  EXPECT_EQ(val0, scalar);

  // Update variable and wait for update to be published
  auto update_1 = scalar;
  update_1["value"].ConvertFrom(42);
  EXPECT_TRUE(m_epics_av_manager.UpdateAnyValue("val0", update_1));
  EXPECT_TRUE(WaitForValue(update_1, 1.0));

  // Serve second set and construct client PV for monitoring
  ASSERT_TRUE(m_epics_av_manager.AddAnyValues(value_set_2));
  sup::epics::PvAccessClientPV val2_pv{"val2", pv_callback};
  EXPECT_TRUE(val2_pv.WaitForValidValue(1.0));
  auto val2 = val2_pv.GetValue();
  EXPECT_EQ(val2, scalar);

  // Update variable and wait for update to be published
  auto update_2 = scalar;
  update_2["value"].ConvertFrom(1999);
  EXPECT_TRUE(m_epics_av_manager.UpdateAnyValue("val2", update_2));
  EXPECT_TRUE(WaitForValue(update_2, 1.0));

  // Check failure to serve variables with known names
  EXPECT_FALSE(m_epics_av_manager.AddAnyValues(value_set_3));

  // Check failure to update variables with unknown names
  EXPECT_FALSE(m_epics_av_manager.UpdateAnyValue("unknown", scalar));
}
