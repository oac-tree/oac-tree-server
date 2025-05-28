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

#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree_server;

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
  auto job_state_value = GetJobStateValue(sup::oac_tree::JobState::kFailed);
  EXPECT_EQ(job_state_value.GetType(), kJobStateAnyValue.GetType());
  EXPECT_EQ(job_state_value.GetTypeName(), kJobStateType);
  ASSERT_TRUE(job_state_value.HasField(kJobStateField));
  EXPECT_EQ(job_state_value[kJobStateField].As<sup::dto::uint32>(),
            static_cast<sup::dto::uint32>(sup::oac_tree::JobState::kFailed));
}

TEST_F(SupAutoProtocolTest, EncodeVariableState)
{
  const sup::dto::AnyValue var_value = {{
    { "flag", true },
    { "setpoint", 42.0 }
  }};
  auto encoded = EncodeVariableState(var_value, true);
  ASSERT_TRUE(encoded.HasField(kVariableValueField));
  ASSERT_TRUE(encoded.HasField(kVariableConnectedField));
  EXPECT_EQ(encoded[kVariableValueField], var_value);
  EXPECT_EQ(encoded[kVariableConnectedField], true);
}

TEST_F(SupAutoProtocolTest, ParseValueName)
{
  {
    // Job state field with prefix is correctly parsed as such
    std::string val_name = "prefix:" + kJobStateId;
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kJobStatus);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Job state field with single character prefix is correctly parsed as such
    std::string val_name = "p" + kJobStateId;
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kJobStatus);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Job state field without prefix is parsed as unknown
    auto info = ParseValueName(kJobStateId);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Log entry field with prefix is correctly parsed as such
    std::string val_name = "prefix:" + kLogEntryId;
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kLogEntry);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Log entry field with single character prefix is correctly parsed as such
    std::string val_name = "p" + kLogEntryId;
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kLogEntry);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Log entry field without prefix is parsed as unknown
    auto info = ParseValueName(kLogEntryId);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Message entry field with prefix is correctly parsed as such
    std::string val_name = "prefix:" + kMessageEntryId;
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kMessageEntry);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Message entry field with single character prefix is correctly parsed as such
    std::string val_name = "p" + kMessageEntryId;
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kMessageEntry);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Message entry field without prefix is parsed as unknown
    auto info = ParseValueName(kMessageEntryId);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Output value entry field with prefix is correctly parsed as such
    std::string val_name = "prefix:" + kOutputValueEntryId;
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kOutputValueEntry);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Output value entry field with single character prefix is correctly parsed as such
    std::string val_name = "p" + kOutputValueEntryId;
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kOutputValueEntry);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Output value entry field without prefix is parsed as unknown
    auto info = ParseValueName(kOutputValueEntryId);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Instruction state field correctly parsed
    std::string val_name = "prefix:" + kInstructionId + "42";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kInstruction);
    EXPECT_EQ(info.idx, 42);
  }
  {
    // Instruction state field with single character prefix correctly parsed
    std::string val_name = "p" + kInstructionId + "17";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kInstruction);
    EXPECT_EQ(info.idx, 17);
  }
  {
    // Instruction state field with zero index correctly parsed
    std::string val_name = "prefix:" + kInstructionId + "0";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kInstruction);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Instruction state field with max index correctly parsed
    auto max_32 = std::numeric_limits<sup::dto::uint32>::max();
    std::string val_name = "prefix:" + kInstructionId + std::to_string(max_32);
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kInstruction);
    EXPECT_EQ(info.idx, max_32);
  }
  {
    // Instruction state field without prefix is parsed as unknown
    std::string val_name = kInstructionId + "42";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Instruction state field with bad integer encoding is parsed as unknown
    std::string val_name = "prefix:" + kInstructionId + "abc";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Instruction state field where index part is not fully parsed as a number is parsed as unknown
    std::string val_name = "prefix:" + kInstructionId + " 12";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Instruction state field where index does not fit 32bit unsigned is parsed as unknown
    std::string val_name = "prefix:" + kInstructionId + "4294967296";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Variable state field correctly parsed
    std::string val_name = "prefix:" + kVariableId + "42";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kVariable);
    EXPECT_EQ(info.idx, 42);
  }
  {
    // Variable state field with single character prefix correctly parsed
    std::string val_name = "p" + kVariableId + "17";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kVariable);
    EXPECT_EQ(info.idx, 17);
  }
  {
    // Variable state field with zero index correctly parsed
    std::string val_name = "prefix:" + kVariableId + "0";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kVariable);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Variable state field with max index correctly parsed
    auto max_32 = std::numeric_limits<sup::dto::uint32>::max();
    std::string val_name = "prefix:" + kVariableId + std::to_string(max_32);
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kVariable);
    EXPECT_EQ(info.idx, max_32);
  }
  {
    // Variable state field without prefix is parsed as unknown
    std::string val_name = kVariableId + "42";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Variable state field with bad integer encoding is parsed as unknown
    std::string val_name = "prefix:" + kVariableId + "abc";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Variable state field where index part is not fully parsed as a number is parsed as unknown
    std::string val_name = "prefix:" + kVariableId + " 12";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Variable state field where index does not fit 32bit unsigned is parsed as unknown
    std::string val_name = "prefix:" + kVariableId + "4294967296";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
  {
    // Unknown type field is parsed as unknown
    std::string val_name = "prefix:unknown-42";
    auto info = ParseValueName(val_name);
    EXPECT_EQ(info.val_type, ValueNameType::kUnknown);
    EXPECT_EQ(info.idx, 0);
  }
}
