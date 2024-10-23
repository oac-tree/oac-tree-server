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

#include <sup/auto-server/anyvalue_io_helper.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Exactly;

using namespace sup::auto_server;

class AnyValueIOHelperTest : public ::testing::Test
{
protected:
  AnyValueIOHelperTest() = default;
  virtual ~AnyValueIOHelperTest() = default;

  using StrictMockAnyValueIO = ::testing::StrictMock<UnitTestHelper::MockAnyValueIO>;
  StrictMockAnyValueIO m_anyvalue_io;
};

TEST_F(AnyValueIOHelperTest, InitializeJobAndVariables)
{
  const std::string job_prefix = "test_job_1";
  sup::dto::uint32 n_vars = 12;
  // Set expectations on calling InitializeJobAndVariables:
  auto initial_value_set = GetInitialValueSet(job_prefix, n_vars);
  EXPECT_CALL(m_anyvalue_io, AddAnyValues(initial_value_set)).Times(Exactly(1));
  auto input_server_name = GetInputServerName(job_prefix);
  EXPECT_CALL(m_anyvalue_io, AddInputHandler(input_server_name)).Times(Exactly(1));

  InitializeJobAndVariables(m_anyvalue_io, job_prefix, n_vars);
}

TEST_F(AnyValueIOHelperTest, InitializeInstructions)
{
  const std::string job_prefix = "test_job_2";
  sup::dto::uint32 n_instr = 8;
  // Set expectations on calling InitializeInstructions:
  auto instruction_value_set = GetInstructionValueSet(job_prefix, n_instr);
  EXPECT_CALL(m_anyvalue_io, AddAnyValues(instruction_value_set)).Times(Exactly(1));

  InitializeInstructions(m_anyvalue_io, job_prefix, n_instr);
}
