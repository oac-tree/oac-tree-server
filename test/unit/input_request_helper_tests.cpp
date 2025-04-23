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

#include <sup/oac-tree-server/input_request_helper.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree_server;

using sup::oac_tree::InputRequestType;
using sup::oac_tree::kInvalidUserInputRequest;

class InputRequestHelperTest : public ::testing::Test
{
protected:
  InputRequestHelperTest() = default;
  virtual ~InputRequestHelperTest() = default;
};

TEST_F(InputRequestHelperTest, EncodeInputRequest)
{
  sup::dto::AnyValue value{ sup::dto::UnsignedInteger32Type, 42u };
  std::string description = "Give a number";
  auto request = sup::oac_tree::CreateUserValueRequest(value, description);

  // Encode
  sup::dto::uint64 id = 32u;
  auto encoded = EncodeInputRequest(id, request);

  ASSERT_TRUE(encoded.HasField(kInputRequestIndexField));
  EXPECT_EQ(encoded[kInputRequestIndexField], id);
  ASSERT_TRUE(encoded.HasField(kInputRequestTypeField));
  ASSERT_TRUE(encoded.HasField(kInputRequestMetadataField));
  ASSERT_TRUE(encoded.HasField(kInputRequestInputTypeField));

  auto decoded = DecodeInputRequest(encoded);
  EXPECT_EQ(std::get<0>(decoded), true);
  EXPECT_EQ(std::get<1>(decoded), id);
  EXPECT_EQ(std::get<2>(decoded), request);
}

TEST_F(InputRequestHelperTest, DecodeInputRequest)
{
  {
    // Successful decoding
    sup::dto::uint64 req_id = 42u;
    sup::dto::AnyValue metadata{ sup::dto::StringType, "meta" };
    const std::string json_type = R"RAW({"type":"uint16"})RAW";
    sup::dto::AnyValue payload_av{{
      { kInputRequestIndexField, req_id },
      { kInputRequestTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice)},
      { kInputRequestMetadataField, metadata },
      { kInputRequestInputTypeField, { sup::dto::StringType, json_type }}
    }};
    // Decode
    auto decoded = DecodeInputRequest(payload_av);
    EXPECT_EQ(std::get<0>(decoded), true);
    EXPECT_EQ(std::get<1>(decoded), req_id);
    auto input_req = std::get<2>(decoded);
    EXPECT_EQ(input_req.m_request_type, InputRequestType::kUserChoice);
    EXPECT_EQ(input_req.m_meta_data, metadata);
    EXPECT_EQ(input_req.m_input_type, sup::dto::UnsignedInteger16Type);
  }
  {
    // Unable to decode
    sup::dto::AnyValue payload_av{ sup::dto::UnsignedInteger32Type, 1u };
    // Decode
    auto decoded = DecodeInputRequest(payload_av);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputRequest);
  }
  {
    // Missing request index field
    sup::dto::AnyValue metadata{ sup::dto::StringType, "meta" };
    const std::string json_type = R"RAW({"type":"uint16"})RAW";
    sup::dto::AnyValue payload_av{{
      { kInputRequestTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice) },
      { kInputRequestMetadataField, metadata },
      { kInputRequestInputTypeField, { sup::dto::StringType, json_type }}
    }};
    // Decode
    auto decoded = DecodeInputRequest(payload_av);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputRequest);
  }
  {
    // Wrong type of request index field
    sup::dto::uint64 req_id = 42u;
    sup::dto::AnyValue metadata{ sup::dto::StringType, "meta" };
    const std::string json_type = R"RAW({"type":"uint16"})RAW";
    sup::dto::AnyValue payload_av{{
      { kInputRequestIndexField, { sup::dto::SignedInteger16Type, req_id }},
      { kInputRequestTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice) },
      { kInputRequestMetadataField, metadata },
      { kInputRequestInputTypeField, { sup::dto::StringType, json_type }}
    }};
    // Decode
    auto decoded = DecodeInputRequest(payload_av);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputRequest);
  }
  {
    // Missing input request type field
    sup::dto::uint64 req_id = 42u;
    sup::dto::AnyValue metadata{ sup::dto::StringType, "meta" };
    const std::string json_type = R"RAW({"type":"uint16"})RAW";
    sup::dto::AnyValue payload_av{{
      { kInputRequestIndexField, req_id },
      { kInputRequestMetadataField, metadata },
      { kInputRequestInputTypeField, { sup::dto::StringType, json_type }}
    }};
    // Decode
    auto decoded = DecodeInputRequest(payload_av);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputRequest);
  }
  {
    // Wrong type of input request type field
    sup::dto::uint64 req_id = 42u;
    sup::dto::AnyValue metadata{ sup::dto::StringType, "meta" };
    const std::string json_type = R"RAW({"type":"uint16"})RAW";
    sup::dto::AnyValue payload_av{{
      { kInputRequestIndexField, req_id },
      { kInputRequestTypeField, { sup::dto::Float32Type, 1.0f }},
      { kInputRequestMetadataField, metadata },
      { kInputRequestInputTypeField, { sup::dto::StringType, json_type }}
    }};
    // Decode
    auto decoded = DecodeInputRequest(payload_av);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputRequest);
  }
  {
    // Missing metadata field
    sup::dto::uint64 req_id = 42u;
    const std::string json_type = R"RAW({"type":"uint16"})RAW";
    sup::dto::AnyValue payload_av{{
      { kInputRequestIndexField, req_id },
      { kInputRequestTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice) },
      { kInputRequestInputTypeField, { sup::dto::StringType, json_type }}
    }};
    // Decode
    auto decoded = DecodeInputRequest(payload_av);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputRequest);
  }
  {
    // Missing input type field
    sup::dto::uint64 req_id = 42u;
    sup::dto::AnyValue metadata{ sup::dto::StringType, "meta" };
    sup::dto::AnyValue payload_av{{
      { kInputRequestIndexField, req_id },
      { kInputRequestTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice) },
      { kInputRequestMetadataField, metadata }
    }};
    // Decode
    auto decoded = DecodeInputRequest(payload_av);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputRequest);
  }
  {
    // Wrong type of input type field
    sup::dto::uint64 req_id = 42u;
    sup::dto::AnyValue metadata{ sup::dto::StringType, "meta" };
    sup::dto::AnyValue payload_av{{
      { kInputRequestIndexField, req_id },
      { kInputRequestTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice) },
      { kInputRequestMetadataField, metadata },
      { kInputRequestInputTypeField, { sup::dto::BooleanType, true }}
    }};
    // Decode
    auto decoded = DecodeInputRequest(payload_av);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputRequest);
  }
  {
    // Input type field cannot be parsed to AnyType
    sup::dto::uint64 req_id = 42u;
    sup::dto::AnyValue metadata{ sup::dto::StringType, "meta" };
    const std::string json_type = R"RAW({"type":"does_not_exist"})RAW";
    sup::dto::AnyValue payload_av{{
      { kInputRequestIndexField, req_id },
      { kInputRequestTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice) },
      { kInputRequestMetadataField, metadata },
      { kInputRequestInputTypeField, { sup::dto::StringType, json_type }}
    }};
    // Decode
    auto decoded = DecodeInputRequest(payload_av);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputRequest);
  }
}
