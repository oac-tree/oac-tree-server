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

#include <sup/auto-server/client_anyvalue_manager.h>
#include <sup/auto-server/server_job_info_io.h>

#include "unit_test_helper.h"

#include <gtest/gtest.h>

using namespace sup::auto_server;

class AnyValueManagerServerClientTest : public ::testing::Test
{
protected:
  AnyValueManagerServerClientTest() = default;
  virtual ~AnyValueManagerServerClientTest() = default;
};

TEST_F(AnyValueManagerServerClientTest, dummy)
{
  EXPECT_TRUE(true);
}
