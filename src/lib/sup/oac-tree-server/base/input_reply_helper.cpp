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
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include <sup/oac-tree-server/input_reply_helper.h>

#include <sup/oac-tree-server/exceptions.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/json_type_parser.h>
#include <sup/oac-tree/anyvalue_utils.h>

namespace
{
bool ValidateInputReplyPayload(const sup::dto::AnyValue& payload);
}  // unnamed namespace

namespace sup
{
namespace oac_tree_server
{
using sup::oac_tree::utils::ValidateMemberType;
using namespace sup::oac_tree;

sup::dto::AnyValue EncodeInputReply(sup::dto::uint64 id, const UserInputReply& input_reply)
{
  sup::dto::AnyValue reply_type_av{ sup::dto::UnsignedInteger32Type,
                                    static_cast<sup::dto::uint32>(input_reply.m_request_type)};
  dto::AnyValue payload = {{
    { kInputReplyIndexField, { sup::dto::UnsignedInteger64Type, id }},
    { kInputReplyTypeField, reply_type_av },
    { kInputReplyResultField, { sup::dto::BooleanType, input_reply.m_result }},
    { kInputReplyPayloadField, input_reply.m_payload }
  }};
  return Base64EncodeAnyValue(payload);
}

std::tuple<bool, sup::dto::uint64, UserInputReply> DecodeInputReply(
  const sup::dto::AnyValue& encoded)
{
  const std::tuple<bool, sup::dto::uint64, UserInputReply> failure{ false, 0,
                                                                    kInvalidUserInputReply };
  auto [decoded, payload] = Base64DecodeAnyValue(encoded);
  if (!decoded)
  {
    return failure;
  }
  if (!ValidateInputReplyPayload(payload))
  {
    return failure;
  }
  auto req_idx = payload[kInputReplyIndexField].As<sup::dto::uint64>();
  InputRequestType request_type =
    static_cast<InputRequestType>(payload[kInputReplyTypeField].As<sup::dto::uint32>());
  bool result = payload[kInputReplyResultField].As<sup::dto::boolean>();
  UserInputReply input_reply{ request_type, result, payload[kInputReplyPayloadField] };
  return { true, req_idx, input_reply };
}

}  // namespace oac_tree_server

}  // namespace sup

namespace
{
using namespace sup::oac_tree_server;
bool ValidateInputReplyPayload(const sup::dto::AnyValue& payload)
{
  if (!ValidateMemberType(payload, kInputReplyIndexField, sup::dto::UnsignedInteger64Type))
  {
    return false;
  }
  if (!ValidateMemberType(payload, kInputReplyTypeField, sup::dto::UnsignedInteger32Type))
  {
    return false;
  }
  if (!ValidateMemberType(payload, kInputReplyResultField, sup::dto::BooleanType))
  {
    return false;
  }
  if (!payload.HasField(kInputReplyPayloadField))
  {
    return false;
  }
  return true;

}
}  // unnamed namespace

