/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : Unit test code
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

#include <sup/oac-tree-server/output_entry_helper.h>
#include <sup/oac-tree-server/output_entry_types.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree_server;

class OutputEntriesTest : public ::testing::Test
{
protected:
  OutputEntriesTest() = default;
  virtual ~OutputEntriesTest() = default;
};

TEST_F(OutputEntriesTest, LogEntrySerialization)
{
  LogEntry original{ 42u, 1729, "hello"};
  auto av = EncodeLogEntry(original);
  EXPECT_TRUE(ValidateLogEntryAnyValue(av));
  auto [decoded, log_entry] = DecodeLogEntry(av);
  ASSERT_TRUE(decoded);
  EXPECT_EQ(log_entry, original);
}

TEST_F(OutputEntriesTest, MessageEntrySerialization)
{
  MessageEntry original{ 42u, "hello"};
  auto av = EncodeMessageEntry(original);
  EXPECT_TRUE(ValidateMessageEntryAnyValue(av));
  auto [decoded, msg_entry] = DecodeMessageEntry(av);
  ASSERT_TRUE(decoded);
  EXPECT_EQ(msg_entry, original);
}

TEST_F(OutputEntriesTest, OutputValueEntrySerialization)
{
  OutputValueEntry original{ 42u, "hello", { sup::dto::BooleanType, true }};
  auto av = EncodeOutputValueEntry(original);
  EXPECT_TRUE(ValidateOutputValueEntryAnyValue(av));
  auto [decoded, out_entry] = DecodeOutputValueEntry(av);
  ASSERT_TRUE(decoded);
  EXPECT_EQ(out_entry, original);
}
