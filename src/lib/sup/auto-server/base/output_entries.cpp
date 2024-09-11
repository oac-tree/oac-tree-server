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

#include <sup/auto-server/output_entries.h>

#include <sup/auto-server/anyvalue_utils.h>
#include <sup/auto-server/sup_auto_protocol.h>

namespace
{
bool ValidateLogEntryAnyValue(const sup::dto::AnyValue& anyvalue);
bool ValidateMessageEntryAnyValue(const sup::dto::AnyValue& anyvalue);
bool ValidateOutputValueEntryAnyValue(const sup::dto::AnyValue& anyvalue);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{

sup::dto::AnyValue EncodeLogEntry(const LogEntry& log_entry)
{
  auto result = kLogEntryAnyValue;
  result[kIndexField] = log_entry.m_index;
  result[kSeverityField] = log_entry.m_severity;
  result[kMessageField] = log_entry.m_message;
  return result;
}

LogEntry DecodeLogEntry(const sup::dto::AnyValue& anyvalue)
{
  LogEntry result = { 0, 0, "" };
  if (ValidateLogEntryAnyValue(anyvalue))
  {
    result.m_index = anyvalue[kIndexField].As<sup::dto::uint64>();
    result.m_severity = anyvalue[kSeverityField].As<sup::dto::int32>();
    result.m_message = anyvalue[kMessageField].As<std::string>();
  }
  return result;
}

sup::dto::AnyValue EncodeMessageEntry(const MessageEntry& msg_entry)
{
  auto result = kMessageEntryAnyValue;
  result[kIndexField] = msg_entry.m_index;
  result[kMessageField] = msg_entry.m_message;
  return result;
}


MessageEntry DecodeMessageEntry(const sup::dto::AnyValue& anyvalue)
{
  MessageEntry result = { 0, "" };
  if (ValidateMessageEntryAnyValue(anyvalue))
  {
    result.m_index = anyvalue[kIndexField].As<sup::dto::uint64>();
    result.m_message = anyvalue[kMessageField].As<std::string>();
  }
  return result;
}

sup::dto::AnyValue EncodeOutputValueEntry(const OutputValueEntry& output_entry)
{
  auto result = kOutputValueEntryAnyValue;
  result[kIndexField] = output_entry.m_index;
  result[kDescriptionField] = output_entry.m_description;
  result[kValueField] = output_entry.m_value;
  return result;
}

OutputValueEntry DecodeOutputValueEntry(const sup::dto::AnyValue& anyvalue)
{
  OutputValueEntry result = { 0, "", {} };
  if (ValidateOutputValueEntryAnyValue(anyvalue))
  {
    result.m_index = anyvalue[kIndexField].As<sup::dto::uint64>();
    result.m_description = anyvalue[kDescriptionField].As<std::string>();
    result.m_value = anyvalue[kValueField];
  }
  return result;
}

}  // namespace auto_server

}  // namespace sup

namespace
{
using namespace sup::auto_server;

bool ValidateLogEntryAnyValue(const sup::dto::AnyValue& anyvalue)
{
  if (!utils::ValidateMemberType(anyvalue, kIndexField, sup::dto::UnsignedInteger64Type))
  {
    return false;
  }
  if (!utils::ValidateMemberType(anyvalue, kSeverityField, sup::dto::SignedInteger32Type))
  {
    return false;
  }
  if (!utils::ValidateMemberType(anyvalue, kMessageField, sup::dto::StringType))
  {
    return false;
  }
  return true;
}

bool ValidateMessageEntryAnyValue(const sup::dto::AnyValue& anyvalue)
{
  if (!utils::ValidateMemberType(anyvalue, kIndexField, sup::dto::UnsignedInteger64Type))
  {
    return false;
  }
  if (!utils::ValidateMemberType(anyvalue, kMessageField, sup::dto::StringType))
  {
    return false;
  }
  return true;
}

bool ValidateOutputValueEntryAnyValue(const sup::dto::AnyValue& anyvalue)
{
  if (!utils::ValidateMemberType(anyvalue, kIndexField, sup::dto::UnsignedInteger64Type))
  {
    return false;
  }
  if (!utils::ValidateMemberType(anyvalue, kDescriptionField, sup::dto::StringType))
  {
    return false;
  }
  if (!anyvalue.HasField(kValueField))
  {
    return false;
  }
  return true;
}

}  // unnamed namespace

