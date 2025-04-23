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

#include <sup/oac-tree-server/output_entry_types.h>

namespace sup
{
namespace oac_tree_server
{

bool operator==(const LogEntry& left, const LogEntry& right)
{
  if (left.m_index != right.m_index)
  {
    return false;
  }
  if (left.m_severity != right.m_severity)
  {
    return false;
  }
  if (left.m_message != right.m_message)
  {
    return false;
  }
  return true;
}

bool operator!=(const LogEntry& left, const LogEntry& right)
{
  return !(left == right);
}

bool operator==(const MessageEntry& left, const MessageEntry& right)
{
  if (left.m_index != right.m_index)
  {
    return false;
  }
  if (left.m_message != right.m_message)
  {
    return false;
  }
  return true;
}

bool operator!=(const MessageEntry& left, const MessageEntry& right)
{
  return !(left == right);
}

bool operator==(const OutputValueEntry& left, const OutputValueEntry& right)
{
  if (left.m_index != right.m_index)
  {
    return false;
  }
  if (left.m_description != right.m_description)
  {
    return false;
  }
  if (left.m_value != right.m_value)
  {
    return false;
  }
  return true;
}

bool operator!=(const OutputValueEntry& left, const OutputValueEntry& right)
{
  return !(left == right);
}

}  // namespace oac_tree_server

}  // namespace sup
