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

#include "unit_test_helper.h"

#include <sup/auto-server/job_commands.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class JobCommandTest : public ::testing::Test
{
protected:
  JobCommandTest() = default;
  virtual ~JobCommandTest() = default;

};

TEST_F(JobCommandTest, Comparison)
{
  EXPECT_LT(JobCommand::kStart, JobCommand::kStep);
  EXPECT_LT(JobCommand::kStart, JobCommand::kPause);
  EXPECT_LT(JobCommand::kPause, JobCommand::kReset);
  EXPECT_LT(JobCommand::kReset, JobCommand::kHalt);
  EXPECT_LT(JobCommand::kHalt, JobCommand::kTerminate);
  EXPECT_FALSE(JobCommand::kTerminate <= JobCommand::kHalt);
  EXPECT_FALSE(JobCommand::kHalt <= JobCommand::kPause);
}
