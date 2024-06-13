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

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/auto-server/base/variable_utils.h>

#include <sup/sequencer/sequence_parser.h>
#include <sup/sequencer/workspace.h>

#include <gtest/gtest.h>

#include <algorithm>

using namespace sup::auto_server;

class VariableUtilsTest : public ::testing::Test
{
protected:
  VariableUtilsTest() = default;
  virtual ~VariableUtilsTest() = default;
};

TEST_F(VariableUtilsTest, VariableInfo)
{
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);

  // Build AnyValue representation of the workspace
  auto& workspace = proc->GetWorkspace();
  auto var_names = workspace.VariableNames();
  auto ws_info = utils::BuildWorkspaceInfoAnyValue(workspace);

  // UnitTestHelper::DumpAnyValue(ws_info);

  auto var_name_map = utils::BuildVariableNameMap(ws_info);
  EXPECT_EQ(var_name_map.size(), var_names.size());
  EXPECT_TRUE(std::is_permutation(var_name_map.begin(), var_name_map.end(), var_names.begin()));
}
