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

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>
#include <sup/auto-server/variable_utils.h>

#include <sup/sequencer/sequence_parser.h>
#include <sup/sequencer/variable.h>
#include <sup/sequencer/variable_registry.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class WorkspaceInfoTest : public ::testing::Test
{
protected:
  WorkspaceInfoTest() = default;
  virtual ~WorkspaceInfoTest() = default;

  VariableInfo CreateTestVariableInfo(sup::dto::uint32 idx)
  {
    return VariableInfo{"TestVariable", idx, {} };
  }
};

const std::string var_name_1 = "var_name_1";
const std::string var_name_2 = "var_name_2";
const std::string var_name_3 = "var_name_3";

TEST_F(WorkspaceInfoTest, AddVariableInfo)
{
  {
    // Valid addition of VariableInfo objects
    WorkspaceInfo ws_info{};
    EXPECT_EQ(ws_info.GetNumberOfVariables(), 0);
    EXPECT_NO_THROW(ws_info.AddVariableInfo(var_name_1, CreateTestVariableInfo(0)));
    EXPECT_NO_THROW(ws_info.AddVariableInfo(var_name_2, CreateTestVariableInfo(1)));
    EXPECT_NO_THROW(ws_info.AddVariableInfo(var_name_3, CreateTestVariableInfo(2)));
    EXPECT_EQ(ws_info.GetNumberOfVariables(), 3);
    const auto& var_infos = ws_info.GetVariableInfos();
    EXPECT_EQ(var_infos.size(), 3);
  }
  {
    // Duplicate names throws
    WorkspaceInfo ws_info{};
    EXPECT_EQ(ws_info.GetNumberOfVariables(), 0);
    EXPECT_NO_THROW(ws_info.AddVariableInfo(var_name_1, CreateTestVariableInfo(0)));
    EXPECT_THROW(ws_info.AddVariableInfo(var_name_1, CreateTestVariableInfo(1)),
                 InvalidOperationException);
    EXPECT_EQ(ws_info.GetNumberOfVariables(), 1);
    const auto& var_infos = ws_info.GetVariableInfos();
    EXPECT_EQ(var_infos.size(), 1);
  }
  {
    // Non-consecutive indices throws
    WorkspaceInfo ws_info{};
    EXPECT_EQ(ws_info.GetNumberOfVariables(), 0);
    EXPECT_NO_THROW(ws_info.AddVariableInfo(var_name_1, CreateTestVariableInfo(0)));
    EXPECT_NO_THROW(ws_info.AddVariableInfo(var_name_2, CreateTestVariableInfo(1)));
    EXPECT_THROW(ws_info.AddVariableInfo(var_name_3, CreateTestVariableInfo(0)),
                 InvalidOperationException);
    EXPECT_EQ(ws_info.GetNumberOfVariables(), 2);
    const auto& var_infos = ws_info.GetVariableInfos();
    EXPECT_EQ(var_infos.size(), 2);
  }
}

TEST_F(WorkspaceInfoTest, WorkspaceInfoFromVariable)
{
  // Construct procedure and extract Workspace
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());
  auto& ws = proc->GetWorkspace();

  // Create WorkspaceInfo and validate
  auto ws_info = utils::CreateWorkspaceInfo(ws);
  EXPECT_EQ(ws_info.GetNumberOfVariables(), 3);
  const auto& vars = ws_info.GetVariableInfos();
  ASSERT_EQ(vars.size(), 3);
  {
    // Test first variable info
    EXPECT_EQ(vars[0].first, "one");
    const auto& var_info = vars[0].second;
    EXPECT_EQ(var_info.GetType(), "Local");
    EXPECT_EQ(var_info.GetIndex(), 0u);
    auto var_attributes = var_info.GetAttributes();
    EXPECT_EQ(var_attributes.size(), 3);
  }
  {
    // Test second variable info
    EXPECT_EQ(vars[1].first, "var1");
    const auto& var_info = vars[1].second;
    EXPECT_EQ(var_info.GetType(), "Local");
    EXPECT_EQ(var_info.GetIndex(), 1u);
    auto var_attributes = var_info.GetAttributes();
    EXPECT_EQ(var_attributes.size(), 3);
  }
  {
    // Test third variable info
    EXPECT_EQ(vars[2].first, "var2");
    const auto& var_info = vars[2].second;
    EXPECT_EQ(var_info.GetType(), "Local");
    EXPECT_EQ(var_info.GetIndex(), 2u);
    auto var_attributes = var_info.GetAttributes();
    EXPECT_EQ(var_attributes.size(), 3);
  }
}

TEST_F(WorkspaceInfoTest, WorkspaceInfoToFromAnyValue)
{
  // Construct procedure and extract Workspace
  const auto procedure_string = UnitTestHelper::CreateProcedureString(kWorkspaceSequenceBody);
  auto proc = sup::sequencer::ParseProcedureString(procedure_string);
  ASSERT_NE(proc.get(), nullptr);
  EXPECT_NO_THROW(proc->Setup());
  auto& ws = proc->GetWorkspace();

  // Create WorkspaceInfo and validate
  auto ws_info = utils::CreateWorkspaceInfo(ws);
  auto ws_info_av = utils::ToAnyValue(ws_info);
  auto ws_info_read_back = utils::ToWorkspaceInfo(ws_info_av);
  EXPECT_EQ(ws_info_read_back, ws_info);
}

// TODO: add tests for failing parsing of AnyValue to WorkspaceInfo