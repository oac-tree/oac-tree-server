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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/oac-tree-server/input_reply_helper.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/protocol/base64_variable_codec.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree_server;

using sup::oac_tree::InputRequestType;
using sup::oac_tree::kInvalidUserInputReply;

class InputReplyHelperTest : public ::testing::Test
{
protected:
  InputReplyHelperTest() = default;
  virtual ~InputReplyHelperTest() = default;
};

TEST_F(InputReplyHelperTest, EncodeInputReply)
{
  sup::dto::AnyValue value{ sup::dto::UnsignedInteger32Type, 42u };
  auto reply = sup::oac_tree::CreateUserValueReply(true, value);

  // Encode
  sup::dto::uint64 id = 32u;
  auto encoded = EncodeInputReply(id, reply);

  ASSERT_TRUE(encoded.HasField(sup::protocol::kEncodingField));
  EXPECT_EQ(encoded[sup::protocol::kEncodingField], sup::protocol::kBase64Encoding);
  ASSERT_TRUE(encoded.HasField(sup::protocol::kValueField));
  EXPECT_EQ(encoded[sup::protocol::kValueField].GetType(), sup::dto::StringType);
  auto decoded = DecodeInputReply(encoded);
  EXPECT_EQ(std::get<0>(decoded), true);
  EXPECT_EQ(std::get<1>(decoded), id);
  EXPECT_EQ(std::get<2>(decoded), reply);
}

TEST_F(InputReplyHelperTest, DecodeInputReply)
{
  sup::dto::uint64 req_id = 42u;
  sup::dto::AnyValue payload{ sup::dto::StringType, "payload" };
  {
    // Successful decoding
    sup::dto::AnyValue payload_av{{
      { kInputReplyIndexField, req_id },
      { kInputReplyTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice)},
      { kInputReplyResultField, true },
      { kInputReplyPayloadField, payload }
    }};
    // Encode explicitly
    auto encoded = Base64EncodeAnyValue(payload_av);
    // Decode
    auto decoded = DecodeInputReply(encoded);
    EXPECT_EQ(std::get<0>(decoded), true);
    EXPECT_EQ(std::get<1>(decoded), req_id);
    auto input_reply = std::get<2>(decoded);
    EXPECT_EQ(input_reply.m_request_type, InputRequestType::kUserChoice);
    EXPECT_EQ(input_reply.m_result, true);
    EXPECT_EQ(input_reply.m_payload, payload);
  }
  {
    // Unable to decode
    sup::dto::AnyValue payload_av{ sup::dto::UnsignedInteger32Type, 1u };
    // Decode
    auto decoded = DecodeInputReply(payload_av);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputReply);
  }
  {
    // Missing reply index field
    sup::dto::AnyValue payload_av{{
      { kInputReplyTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice)},
      { kInputReplyResultField, true },
      { kInputReplyPayloadField, payload }
    }};
    // Encode explicitly
    auto encoded = Base64EncodeAnyValue(payload_av);
    // Decode
    auto decoded = DecodeInputReply(encoded);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputReply);
  }
  {
    // Wrong type of reply index field
    sup::dto::AnyValue payload_av{{
      { kInputReplyIndexField, { sup::dto::SignedInteger16Type, req_id }},
      { kInputReplyTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice)},
      { kInputReplyResultField, true },
      { kInputReplyPayloadField, payload }
    }};
    // Encode explicitly
    auto encoded = Base64EncodeAnyValue(payload_av);
    // Decode
    auto decoded = DecodeInputReply(encoded);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputReply);
  }
  {
    // Missing input reply type field
    sup::dto::AnyValue payload_av{{
      { kInputReplyIndexField, req_id },
      { kInputReplyResultField, true },
      { kInputReplyPayloadField, payload }
    }};
    // Encode explicitly
    auto encoded = Base64EncodeAnyValue(payload_av);
    // Decode
    auto decoded = DecodeInputReply(encoded);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputReply);
  }
  {
    // Wrong type of input reply type field
    sup::dto::AnyValue payload_av{{
      { kInputReplyIndexField, req_id },
      { kInputReplyTypeField, { sup::dto::Float32Type, 1.0f }},
      { kInputReplyResultField, true },
      { kInputReplyPayloadField, payload }
    }};
    // Encode explicitly
    auto encoded = Base64EncodeAnyValue(payload_av);
    // Decode
    auto decoded = DecodeInputReply(encoded);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputReply);
  }
  {
    // Missing result field
    sup::dto::AnyValue payload_av{{
      { kInputReplyIndexField, req_id },
      { kInputReplyTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice)},
      { kInputReplyPayloadField, payload }
    }};
    // Encode explicitly
    auto encoded = Base64EncodeAnyValue(payload_av);
    // Decode
    auto decoded = DecodeInputReply(encoded);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputReply);
  }
  {
    // Wrong type of result field
    sup::dto::AnyValue payload_av{{
      { kInputReplyIndexField, req_id },
      { kInputReplyTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice)},
      { kInputReplyResultField, {sup::dto::SignedInteger16Type, 42} },
      { kInputReplyPayloadField, payload }
    }};
    // Encode explicitly
    auto encoded = Base64EncodeAnyValue(payload_av);
    // Decode
    auto decoded = DecodeInputReply(encoded);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputReply);
  }
  {
    // Missing payload field
    sup::dto::AnyValue payload_av{{
      { kInputReplyIndexField, req_id },
      { kInputReplyTypeField, static_cast<sup::dto::uint32>(InputRequestType::kUserChoice)},
      { kInputReplyResultField, true }
    }};
    // Encode explicitly
    auto encoded = Base64EncodeAnyValue(payload_av);
    // Decode
    auto decoded = DecodeInputReply(encoded);
    EXPECT_EQ(std::get<0>(decoded), false);
    EXPECT_EQ(std::get<1>(decoded), 0);
    EXPECT_EQ(std::get<2>(decoded), kInvalidUserInputReply);
  }
}
