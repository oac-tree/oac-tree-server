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

#ifndef SUP_AUTO_SERVER_OUTPUT_ENTRIES_H_
#define SUP_AUTO_SERVER_OUTPUT_ENTRIES_H_

#include <sup/dto/anyvalue.h>

#include <string>

namespace sup
{
namespace auto_server
{

struct LogEntry
{
  sup::dto::uint64 m_index;
  sup::dto::int32 m_severity;
  std::string m_message;
};

sup::dto::AnyValue EncodeLogEntry(const LogEntry& log_entry);

LogEntry DecodeLogEntry(const sup::dto::AnyValue& anyvalue);

struct MessageEntry
{
  sup::dto::uint64 m_index;
  std::string m_message;
};

sup::dto::AnyValue EncodeMessageEntry(const MessageEntry& msg_entry);

MessageEntry DecodeMessageEntry(const sup::dto::AnyValue& anyvalue);

struct OutputValueEntry
{
  sup::dto::uint64 m_index;
  std::string m_description;
  sup::dto::AnyValue m_value;
};

sup::dto::AnyValue EncodeOutputValueEntry(const OutputValueEntry& output_entry);

OutputValueEntry DecodeOutputValueEntry(const sup::dto::AnyValue& anyvalue);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_OUTPUT_ENTRIES_H_
