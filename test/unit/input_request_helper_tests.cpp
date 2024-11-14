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

#include <sup/auto-server/input_request_helper.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/protocol/base64_variable_codec.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

using sup::sequencer::InputRequestType;
using sup::sequencer::kInvalidUserInputRequest;

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
  auto request = sup::sequencer::CreateUserValueRequest(value, description);

  // Encode
  sup::dto::uint64 id = 32u;
  auto encoded = EncodeInputRequest(id, request);

  ASSERT_TRUE(encoded.HasField(sup::protocol::kEncodingField));
  EXPECT_EQ(encoded[sup::protocol::kEncodingField], sup::protocol::kBase64Encoding);
  ASSERT_TRUE(encoded.HasField(sup::protocol::kValueField));
  EXPECT_EQ(encoded[sup::protocol::kValueField].GetType(), sup::dto::StringType);
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
    // Encode explicitly
    auto encoded = sup::protocol::Base64VariableCodec::Encode(payload_av);
    EXPECT_TRUE(encoded.first);
    // Decode
    auto decoded = DecodeInputRequest(encoded.second);
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
    // Encode explicitly
    auto encoded = sup::protocol::Base64VariableCodec::Encode(payload_av);
    EXPECT_TRUE(encoded.first);
    // Decode
    auto decoded = DecodeInputRequest(encoded.second);
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
    // Encode explicitly
    auto encoded = sup::protocol::Base64VariableCodec::Encode(payload_av);
    EXPECT_TRUE(encoded.first);
    // Decode
    auto decoded = DecodeInputRequest(encoded.second);
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
    // Encode explicitly
    auto encoded = sup::protocol::Base64VariableCodec::Encode(payload_av);
    EXPECT_TRUE(encoded.first);
    // Decode
    auto decoded = DecodeInputRequest(encoded.second);
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
    // Encode explicitly
    auto encoded = sup::protocol::Base64VariableCodec::Encode(payload_av);
    EXPECT_TRUE(encoded.first);
    // Decode
    auto decoded = DecodeInputRequest(encoded.second);
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
    // Encode explicitly
    auto encoded = sup::protocol::Base64VariableCodec::Encode(payload_av);
    EXPECT_TRUE(encoded.first);
    // Decode
    auto decoded = DecodeInputRequest(encoded.second);
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
    // Encode explicitly
    auto encoded = sup::protocol::Base64VariableCodec::Encode(payload_av);
    EXPECT_TRUE(encoded.first);
    // Decode
    auto decoded = DecodeInputRequest(encoded.second);
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
    // Encode explicitly
    auto encoded = sup::protocol::Base64VariableCodec::Encode(payload_av);
    EXPECT_TRUE(encoded.first);
    // Decode
    auto decoded = DecodeInputRequest(encoded.second);
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
    // Encode explicitly
    auto encoded = sup::protocol::Base64VariableCodec::Encode(payload_av);
    EXPECT_TRUE(encoded.first);
    // Decode
    auto decoded = DecodeInputRequest(encoded.second);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputRequest);
  }
}
