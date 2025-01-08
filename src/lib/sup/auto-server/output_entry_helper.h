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

#ifndef SUP_AUTO_SERVER_OUTPUT_ENTRY_HELPER_H_
#define SUP_AUTO_SERVER_OUTPUT_ENTRY_HELPER_H_

#include <sup/auto-server/output_entry_types.h>

#include <utility>

namespace sup
{
namespace auto_server
{

bool ValidateLogEntryAnyValue(const sup::dto::AnyValue& anyvalue);

sup::dto::AnyValue EncodeLogEntry(const LogEntry& log_entry);

std::pair<bool, LogEntry> DecodeLogEntry(const sup::dto::AnyValue& anyvalue);

bool ValidateMessageEntryAnyValue(const sup::dto::AnyValue& anyvalue);

sup::dto::AnyValue EncodeMessageEntry(const MessageEntry& msg_entry);

std::pair<bool, MessageEntry> DecodeMessageEntry(const sup::dto::AnyValue& anyvalue);

bool ValidateOutputValueEntryAnyValue(const sup::dto::AnyValue& anyvalue);

sup::dto::AnyValue EncodeOutputValueEntry(const OutputValueEntry& output_entry);

std::pair<bool, OutputValueEntry> DecodeOutputValueEntry(const sup::dto::AnyValue& anyvalue);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_OUTPUT_ENTRY_HELPER_H_
