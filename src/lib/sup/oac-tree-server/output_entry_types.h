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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_OAC_TREE_SERVER_OUTPUT_ENTRY_TYPES_H_
#define SUP_OAC_TREE_SERVER_OUTPUT_ENTRY_TYPES_H_

#include <sup/dto/anyvalue.h>

#include <string>

namespace sup
{
namespace oac_tree_server
{

struct LogEntry
{
  sup::dto::uint64 m_index;
  sup::dto::int32 m_severity;
  std::string m_message;
};

bool operator==(const LogEntry& left, const LogEntry& right);
bool operator!=(const LogEntry& left, const LogEntry& right);

struct MessageEntry
{
  sup::dto::uint64 m_index;
  std::string m_message;
};

bool operator==(const MessageEntry& left, const MessageEntry& right);
bool operator!=(const MessageEntry& left, const MessageEntry& right);

struct OutputValueEntry
{
  sup::dto::uint64 m_index;
  std::string m_description;
  sup::dto::AnyValue m_value;
};

bool operator==(const OutputValueEntry& left, const OutputValueEntry& right);
bool operator!=(const OutputValueEntry& left, const OutputValueEntry& right);

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_OUTPUT_ENTRY_TYPES_H_
