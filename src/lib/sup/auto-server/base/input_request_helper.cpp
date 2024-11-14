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

#include <sup/auto-server/input_request_helper.h>

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/dto/anytype_helper.h>
#include <sup/dto/json_type_parser.h>
#include <sup/protocol/base64_variable_codec.h>
#include <sup/sequencer/anyvalue_utils.h>

namespace
{
bool ValidateInputRequestPayload(const sup::dto::AnyValue& payload);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{
using sup::sequencer::utils::ValidateMemberType;
using namespace sup::sequencer;

sup::dto::AnyValue EncodeInputRequest(sup::dto::uint64 id, const UserInputRequest& input_request)
{
  sup::dto::AnyValue request_type_av{ sup::dto::UnsignedInteger32Type,
                                      static_cast<sup::dto::uint32>(input_request.m_request_type)};
  dto::AnyValue payload = {{
    { kInputRequestIndexField, { sup::dto::UnsignedInteger64Type, id }},
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

std::tuple<bool, sup::dto::uint64, UserInputRequest> DecodeInputRequest(
  const sup::dto::AnyValue& encoded)
{
  const std::tuple<bool, sup::dto::uint64, UserInputRequest> failure{ false, 0,
                                                                      kInvalidUserInputRequest };
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
  auto req_idx = payload[kInputRequestIndexField].As<sup::dto::uint64>();
  InputRequestType request_type =
    static_cast<InputRequestType>(payload[kInputRequestTypeField].As<sup::dto::uint32>());
  sup::dto::JSONAnyTypeParser type_parser;
  if (!type_parser.ParseString(payload[kInputRequestInputTypeField].As<std::string>()))
  {
    return failure;
  }
  UserInputRequest input_request{ request_type, payload[kInputRequestMetadataField],
                                  type_parser.MoveAnyType() };
  return { true, req_idx, input_request };
}

}  // namespace auto_server

}  // namespace sup

namespace
{
using namespace sup::auto_server;
bool ValidateInputRequestPayload(const sup::dto::AnyValue& payload)
{
  if (!ValidateMemberType(payload, kInputRequestIndexField, sup::dto::UnsignedInteger64Type))
  {
    return false;
  }
  if (!ValidateMemberType(payload, kInputRequestTypeField, sup::dto::UnsignedInteger32Type))
  {
    return false;
  }
  if (!payload.HasField(kInputRequestMetadataField))
  {
    return false;
  }
  if (!ValidateMemberType(payload, kInputRequestInputTypeField, sup::dto::StringType))
  {
    return false;
  }
  return true;

}
}  // unnamed namespace

