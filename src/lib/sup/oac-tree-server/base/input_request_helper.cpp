/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : oac-tree server
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

#include <sup/oac-tree-server/exceptions.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/dto/anytype_helper.h>
#include <sup/dto/json_type_parser.h>
#include <sup/oac-tree/anyvalue_utils.h>

namespace
{
bool ValidateInputRequestPayload(const sup::dto::AnyValue& payload);
}  // unnamed namespace

namespace sup
{
namespace oac_tree_server
{
using sup::oac_tree::utils::ValidateMemberType;
using namespace sup::oac_tree;

sup::dto::AnyValue EncodeInputRequest(sup::dto::uint64 id, const UserInputRequest& input_request)
{
  sup::dto::AnyValue request_type_av{ sup::dto::UnsignedInteger32Type,
                                      static_cast<sup::dto::uint32>(input_request.m_request_type)};
  dto::AnyValue encoded = {{
    { kInputRequestIndexField, { sup::dto::UnsignedInteger64Type, id }},
    { kInputRequestTypeField, request_type_av },
    { kInputRequestMetadataField, input_request.m_meta_data },
    { kInputRequestInputTypeField, sup::dto::AnyTypeToJSONString(input_request.m_input_type) }
  }};
  return encoded;
}

std::tuple<bool, sup::dto::uint64, UserInputRequest> DecodeInputRequest(
  const sup::dto::AnyValue& encoded)
{
  const std::tuple<bool, sup::dto::uint64, UserInputRequest> failure{ false, 0,
                                                                      kInvalidUserInputRequest };
  if (!ValidateInputRequestPayload(encoded))
  {
    return failure;
  }
  auto req_idx = encoded[kInputRequestIndexField].As<sup::dto::uint64>();
  InputRequestType request_type =
    static_cast<InputRequestType>(encoded[kInputRequestTypeField].As<sup::dto::uint32>());
  sup::dto::JSONAnyTypeParser type_parser;
  if (!type_parser.ParseString(encoded[kInputRequestInputTypeField].As<std::string>()))
  {
    return failure;
  }
  UserInputRequest input_request{ request_type, encoded[kInputRequestMetadataField],
                                  type_parser.MoveAnyType() };
  return { true, req_idx, input_request };
}

}  // namespace oac_tree_server

}  // namespace sup

namespace
{
using namespace sup::oac_tree_server;
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

