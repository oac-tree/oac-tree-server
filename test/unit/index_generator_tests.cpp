/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVER
 *
 * Description   : Unit test code
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

#include <sup/auto-server/index_generator.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

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
