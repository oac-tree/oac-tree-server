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

#include <sup/auto-server/anyvalue_input_request.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class AnyValueInputRequestTest : public ::testing::Test
{
protected:
  AnyValueInputRequestTest() = default;
  virtual ~AnyValueInputRequestTest() = default;
};

TEST_F(AnyValueInputRequestTest, CreateUserValueRequest)
{
  sup::dto::AnyType input_type = {{
    { "a", sup::dto::Float64Type },
    { "indices", sup::dto::AnyType{5, sup::dto::UnsignedInteger16Type }}
  }};
  sup::dto::AnyValue input{input_type};
  std::string description = "Set configuration";
  auto request = CreateUserValueRequest(input, description);

  // Check request fields
  EXPECT_EQ(request.m_request_type, InputRequestType::kUserValue);
  EXPECT_EQ(request.m_meta_data, sup::dto::AnyValue{description});
  EXPECT_EQ(request.m_input_type, input_type);

  // Parse this request
  sup::dto::AnyValue value_parsed{};
  std::string description_parsed{};
  EXPECT_TRUE(ParseUserValueRequest(request, value_parsed, description_parsed));
  EXPECT_EQ(value_parsed, input);
  EXPECT_EQ(description_parsed, description);
}

TEST_F(AnyValueInputRequestTest, CreateUserChoiceRequest)
{
  std::vector<std::string> options{ "start", "stop", "pause", "resume"};
  sup::dto::AnyValue metadata = {{
    { "a", "useful information about request"},
    { "flag", true }
  }};
  auto request = CreateUserChoiceRequest(options, metadata);

  // Check request fields
  EXPECT_EQ(request.m_request_type, InputRequestType::kUserChoice);
  EXPECT_EQ(request.m_input_type, sup::dto::SignedInteger32Type);
  ASSERT_TRUE(request.m_meta_data.HasField(kInputRequestOptionsFieldName));
  ASSERT_TRUE(request.m_meta_data.HasField(kInputRequestMetadataFieldName));
  EXPECT_EQ(request.m_meta_data[kInputRequestMetadataFieldName], metadata);
  auto& options_av = request.m_meta_data[kInputRequestOptionsFieldName];
  ASSERT_TRUE(sup::dto::IsArrayValue(options_av));
  ASSERT_EQ(options_av.GetType().ElementType(), sup::dto::StringType);
  ASSERT_EQ(options_av.NumberOfElements(), options.size());
  for (std::size_t idx = 0; idx < options.size(); ++idx)
  {
    EXPECT_EQ(options_av[idx].As<std::string>(), options[idx]);
  }

  // Parse this request
  std::vector<std::string> options_parsed;
  sup::dto::AnyValue meta_parsed;
  EXPECT_TRUE(ParseUserChoiceRequest(request, options_parsed, meta_parsed));
  EXPECT_EQ(options_parsed, options);
  EXPECT_EQ(meta_parsed, metadata);
}
