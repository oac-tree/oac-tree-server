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

#include <sup/auto-server/output_entry_helper.h>
#include <sup/auto-server/sup_auto_protocol.h>
#include <sup/auto-server/exceptions.h>

#include <sup/sequencer/anyvalue_utils.h>
#include <sup/sequencer/constants.h>

namespace sup
{
namespace auto_server
{
using sup::sequencer::utils::ValidateMemberType;
using sup::sequencer::Constants::kIndexField;

bool ValidateLogEntryAnyValue(const sup::dto::AnyValue& anyvalue)
{
  if (!ValidateMemberType(anyvalue, kIndexField, sup::dto::UnsignedInteger64Type))
  {
    return false;
  }
  if (!ValidateMemberType(anyvalue, kSeverityField, sup::dto::SignedInteger32Type))
  {
    return false;
  }
  if (!ValidateMemberType(anyvalue, kMessageField, sup::dto::StringType))
  {
    return false;
  }
  return true;
}

sup::dto::AnyValue EncodeLogEntry(const LogEntry& log_entry)
{
  auto result = kLogEntryAnyValue;
  result[kIndexField] = log_entry.m_index;
  result[kSeverityField] = log_entry.m_severity;
  result[kMessageField] = log_entry.m_message;
  return result;
}

std::pair<bool, LogEntry> DecodeLogEntry(const sup::dto::AnyValue& anyvalue)
{
  if (!ValidateLogEntryAnyValue(anyvalue))
  {
    return { false, {} };
  }
  LogEntry result{};
  result.m_index = anyvalue[kIndexField].As<sup::dto::uint64>();
  result.m_severity = anyvalue[kSeverityField].As<sup::dto::int32>();
  result.m_message = anyvalue[kMessageField].As<std::string>();
  return { true, result };
}

bool ValidateMessageEntryAnyValue(const sup::dto::AnyValue& anyvalue)
{
  if (!ValidateMemberType(anyvalue, kIndexField, sup::dto::UnsignedInteger64Type))
  {
    return false;
  }
  if (!ValidateMemberType(anyvalue, kMessageField, sup::dto::StringType))
  {
    return false;
  }
  return true;
}

sup::dto::AnyValue EncodeMessageEntry(const MessageEntry& msg_entry)
{
  auto result = kMessageEntryAnyValue;
  result[kIndexField] = msg_entry.m_index;
  result[kMessageField] = msg_entry.m_message;
  return result;
}


std::pair<bool, MessageEntry> DecodeMessageEntry(const sup::dto::AnyValue& anyvalue)
{
  if (!ValidateMessageEntryAnyValue(anyvalue))
  {
    return { false, {} };
  }
  MessageEntry result{};
  result.m_index = anyvalue[kIndexField].As<sup::dto::uint64>();
  result.m_message = anyvalue[kMessageField].As<std::string>();
  return { true, result };
}

bool ValidateOutputValueEntryAnyValue(const sup::dto::AnyValue& anyvalue)
{
  if (!ValidateMemberType(anyvalue, kIndexField, sup::dto::UnsignedInteger64Type))
  {
    return false;
  }
  if (!ValidateMemberType(anyvalue, kDescriptionField, sup::dto::StringType))
  {
    return false;
  }
  if (!anyvalue.HasField(kValueField))
  {
    return false;
  }
  return true;
}

sup::dto::AnyValue EncodeOutputValueEntry(const OutputValueEntry& output_entry)
{
  auto result = kOutputValueEntryAnyValue;
  result[kIndexField] = output_entry.m_index;
  result[kDescriptionField] = output_entry.m_description;
  result[kValueField] = output_entry.m_value;
  return result;
}

std::pair<bool, OutputValueEntry> DecodeOutputValueEntry(const sup::dto::AnyValue& anyvalue)
{
  std::pair<bool, OutputValueEntry> failure = { false, {} };
  if (ValidateOutputValueEntryAnyValue(anyvalue))
  {
    OutputValueEntry result{};
    result.m_index = anyvalue[kIndexField].As<sup::dto::uint64>();
    result.m_description = anyvalue[kDescriptionField].As<std::string>();
    result.m_value = anyvalue[kValueField];
    return { true, result };
  }
  return failure;
}

}  // namespace auto_server

}  // namespace sup
