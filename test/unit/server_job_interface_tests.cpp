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

#include <sup/auto-server/server_job_interface.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

const std::string kTestPrefix = "ServerJobInterfaceTests";

class ServerJobInterfaceTest : public ::testing::Test
{
protected:
  ServerJobInterfaceTest() = default;
  virtual ~ServerJobInterfaceTest() = default;

  UnitTestHelper::TestServerInterface m_test_server_interface;
};

TEST_F(ServerJobInterfaceTest, Construction)
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_EQ(m_test_server_interface.GetSize(), 0);
  ServerJobInterface job_interface{kTestPrefix, *proc, m_test_server_interface};
  // std::cout << m_test_server_interface;

  // Check presence of job state and 3 variable anyvalues:
  EXPECT_EQ(m_test_server_interface.GetSize(), 4);
  EXPECT_TRUE(m_test_server_interface.HasAnyValue(GetJobStatePVName(kTestPrefix)));
  EXPECT_TRUE(m_test_server_interface.HasAnyValue(GetVariablePVName(kTestPrefix, 0)));
  EXPECT_TRUE(m_test_server_interface.HasAnyValue(GetVariablePVName(kTestPrefix, 1)));
  EXPECT_TRUE(m_test_server_interface.HasAnyValue(GetVariablePVName(kTestPrefix, 2)));

  // Check job state anyvalue is initial:
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetJobStatePVName(kTestPrefix)), kJobStateAnyValue);

  // Check variable anyvalues are empty:
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 0)),
            kVariableAnyValue);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 1)),
            kVariableAnyValue);
  EXPECT_EQ(m_test_server_interface.GetAnyValue(GetVariablePVName(kTestPrefix, 2)),
            kVariableAnyValue);
}

