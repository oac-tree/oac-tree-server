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
  for (std::size_t idx = 0; idx < options.size(); ++ idx)
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

}  // namespace auto_server

}  // namespace sup
