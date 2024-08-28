/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVER
 *
 * Description   : SUP automation server
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

#include <sup/auto-server/anyvalue_utils.h>
#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/dto/anytype_helper.h>
#include <sup/dto/json_type_parser.h>
#include <sup/protocol/base64_variable_codec.h>

namespace
{
bool ValidateInputRequestPayload(const sup::dto::AnyValue& payload);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{

AnyValueInputRequest CreateUserValueRequest(const sup::dto::AnyValue& value,
                                            const std::string& description)
{
  sup::dto::AnyValue meta_data = description;
  AnyValueInputRequest input_request{ InputRequestType::kUserValue, meta_data, value.GetType() };
  return input_request;
}

AnyValueInputRequest CreateUserChoiceRequest(const std::vector<std::string>& options,
                                             const sup::dto::AnyValue& metadata)
{
  sup::dto::AnyValue options_av{ options.size(), sup::dto::StringType };
  for (std::size_t idx = 0; idx < options.size(); ++idx)
  {
    options_av[idx] = options[idx];
  }
  sup::dto::AnyValue meta = {{
    { kInputRequestOptionsFieldName, options_av },
    { kInputRequestMetadataFieldName, metadata }
  }};
  AnyValueInputRequest input_request{ InputRequestType::kUserChoice, meta,
                                      sup::dto::SignedInteger32Type};
  return input_request;
}

std::pair<bool, sup::dto::AnyValue> ParseUserValueReply(const sup::dto::AnyValue& reply)
{
  std::pair<bool, sup::dto::AnyValue> failure{ false, {} };
  if (!utils::ValidateMemberType(reply, kInputReplyResultFieldName, sup::dto::BooleanType))
  {
    return failure;
  }
  if (!reply[kInputReplyResultFieldName].As<sup::dto::boolean>())
  {
    return failure;
  }
  if (!reply.HasField(kInputReplyValueFieldName))
  {
    return failure;
  }
  return { true, reply[kInputReplyValueFieldName] };
}

std::pair<bool, int> ParseUserChoiceReply(const sup::dto::AnyValue& reply)
{
  std::pair<bool, int> failure{ false, -1 };
  if (!utils::ValidateMemberType(reply, kInputReplyResultFieldName, sup::dto::BooleanType))
  {
    return failure;
  }
  if (!reply[kInputReplyResultFieldName].As<sup::dto::boolean>())
  {
    return failure;
  }
  if (!utils::ValidateMemberType(reply, kInputReplyValueFieldName, sup::dto::SignedInteger32Type))
  {
    return failure;
  }
  return { true, reply[kInputReplyValueFieldName].As<sup::dto::int32>() };
}

sup::dto::AnyValue EncodeInputRequest(const AnyValueInputRequest& input_request)
{
  sup::dto::AnyValue request_type_av{ sup::dto::UnsignedInteger32Type,
                                      static_cast<sup::dto::uint32>(input_request.m_request_type)};
  dto::AnyValue payload = {{
    { kInputRequestTypeField, request_type_av },
    { kInputRequestMetadataField, input_request.m_meta_data },
    { kInputRequestInputTypeField, sup::dto::AnyTypeToJSONString(input_request.m_input_type) }
  }};
  auto encoded = protocol::Base64VariableCodec::Encode(payload);
  if (!encoded.first)
  {
    const std::string error = "EncodeInputRequest(): could not encode the input request";
    throw InvalidOperationException(error);
  }
  return encoded.second;
}

std::pair<bool, AnyValueInputRequest> DecodeInputRequest(const dto::AnyValue& encoded)
{
  const std::pair<bool, AnyValueInputRequest> failure{ false, {} };
  auto decoded = protocol::Base64VariableCodec::Decode(encoded);
  if (!decoded.first)
  {
    return failure;
  }
  auto& payload = decoded.second;
  if (!ValidateInputRequestPayload(payload))
  {
    return failure;
  }
  InputRequestType request_type =
    static_cast<InputRequestType>(payload[kInputRequestTypeField].As<sup::dto::uint32>());
  sup::dto::JSONAnyTypeParser type_parser;
  if (!type_parser.ParseString(payload[kInputRequestInputTypeField].As<std::string>()))
  {
    return failure;
  }
  AnyValueInputRequest input_request{ request_type, payload[kInputRequestMetadataField],
                                      type_parser.MoveAnyType() };
  return { true, input_request };
}

}  // namespace auto_server

}  // namespace sup

namespace
{
using namespace sup::auto_server;
bool ValidateInputRequestPayload(const sup::dto::AnyValue& payload)
{
  if (!utils::ValidateMemberType(payload, kInputRequestTypeField, sup::dto::UnsignedInteger32Type))
  {
    return false;
  }
  if (!payload.HasField(kInputRequestMetadataField))
  {
    return false;
  }
  if (!utils::ValidateMemberType(payload, kInputRequestInputTypeField, sup::dto::StringType))
  {
    return false;
  }
  return true;

}
}  // unnamed namespace

