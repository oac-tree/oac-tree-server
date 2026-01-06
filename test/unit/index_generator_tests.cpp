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

#include <sup/oac-tree-server/index_generator.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree_server;

class IndexGeneratorTest : public ::testing::Test
{
protected:
  IndexGeneratorTest() = default;
  virtual ~IndexGeneratorTest() = default;
};

TEST_F(IndexGeneratorTest, NewIndex)
{
  IndexGenerator gen{};
  EXPECT_EQ(gen.CurrentIndex(), 0);
  EXPECT_EQ(gen.NewIndex(), 1u);
  EXPECT_EQ(gen.CurrentIndex(), 1);
  EXPECT_EQ(gen.NewIndex(), 2u);
}
