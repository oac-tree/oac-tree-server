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

#include <sup/protocol/base64_variable_codec.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class SupAutoProtocolTest : public ::testing::Test
{
protected:
  SupAutoProtocolTest() = default;
  virtual ~SupAutoProtocolTest() = default;
};

TEST_F(SupAutoProtocolTest, PVNames)
{
  const std::string prefix = "Test_PVNames";

  // Check value names
  EXPECT_EQ(GetJobStatePVName(prefix), prefix + kJobStateId);
  EXPECT_EQ(GetInstructionPVName(prefix, 1729u), prefix + kInstructionId + "1729");
  EXPECT_EQ(GetVariablePVName(prefix, 42u), prefix + kVariableId + "42");
}

TEST_F(SupAutoProtocolTest, JobStateValue)
{
  const std::string prefix = "Test_JobStateValue";

  // Construct procedure
  auto job_state_value = GetJobStateValue(sup::sequencer::JobState::kFailed);
  EXPECT_EQ(job_state_value.GetType(), kJobStateAnyValue.GetType());
  EXPECT_EQ(job_state_value.GetTypeName(), kJobStateType);
  ASSERT_TRUE(job_state_value.HasField(kJobStateField));
  EXPECT_EQ(job_state_value[kJobStateField].As<sup::dto::uint32>(),
            static_cast<sup::dto::uint32>(sup::sequencer::JobState::kFailed));
}

TEST_F(SupAutoProtocolTest, EncodeVariableInfo)
{
  const sup::dto::AnyValue var_value = {{
    { "flag", true },
    { "setpoint", 42.0 }
  }};
  auto encoded = EncodeVariableInfo(var_value, true);
  auto decoded = sup::protocol::Base64VariableCodec::Decode(encoded);
  ASSERT_TRUE(decoded.first);
  auto payload = decoded.second;
  ASSERT_TRUE(payload.HasField(kVariableValueField));
  ASSERT_TRUE(payload.HasField(kVariableConnectedField));
  EXPECT_EQ(payload[kVariableValueField], var_value);
  EXPECT_EQ(payload[kVariableConnectedField], true);
}
