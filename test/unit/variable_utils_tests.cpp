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
#include <sup/auto-server/variable_utils.h>

#include <sup/sequencer/sequence_parser.h>
#include <sup/sequencer/variable.h>
#include <sup/sequencer/variable_registry.h>

#include <gtest/gtest.h>

using namespace sup::auto_server;

class VariableUtilsTest : public ::testing::Test
{
protected:
  VariableUtilsTest() = default;
  virtual ~VariableUtilsTest() = default;
};

TEST_F(VariableUtilsTest, VariableInfoFromVariable)
{
  const std::string var_type = "Local";
  const sup::dto::uint32 var_idx = 42u;
  auto var = sup::sequencer::GlobalVariableRegistry().Create(var_type);

  // Check VariableInfo from Variable without attributes
  auto var_info = utils::CreateVariableInfo(var.get(), var_idx);
  EXPECT_EQ(var_info.GetType(), var_type);
  EXPECT_EQ(var_info.GetIndex(), var_idx);
  auto attributes = var_info.GetAttributes();
  EXPECT_EQ(attributes.size(), 0);

  // Check VariableInfo after adding attributes
  StringAttribute attr_0{"one", "hello"};
  StringAttribute attr_1{"two", "world"};
  var->AddAttribute(attr_0.first, attr_0.second);
  var->AddAttribute(attr_1.first, attr_1.second);
  var_info = utils::CreateVariableInfo(var.get(), var_idx);
  EXPECT_EQ(var_info.GetType(), var_type);
  EXPECT_EQ(var_info.GetIndex(), var_idx);
  attributes = var_info.GetAttributes();
  ASSERT_EQ(attributes.size(), 2);
  EXPECT_EQ(attributes[0], attr_0);
  EXPECT_EQ(attributes[1], attr_1);
}

TEST_F(VariableUtilsTest, WorkspaceInfoFromVariable)
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

TEST_F(VariableUtilsTest, VariableInfoToFromAnyValue)
{
  {
    // Variable without attributes
    std::vector<StringAttribute> attributes;
    VariableInfo var_info{"MyVarType", 33u, attributes};
    auto var_info_av = utils::ToAnyValue(var_info);
    auto var_info_read_back = utils::ToVariableInfo(var_info_av);
    EXPECT_EQ(var_info_read_back, var_info);
  }
  {
    // Variable with two attributes
    StringAttribute attr_0{"one", "hello"};
    StringAttribute attr_1{"two", "world"};
    std::vector<StringAttribute> attributes;
    attributes.push_back(attr_0);
    attributes.push_back(attr_1);
    VariableInfo var_info{"MyVarType", 0, attributes};
    auto var_info_av = utils::ToAnyValue(var_info);
    auto var_info_read_back = utils::ToVariableInfo(var_info_av);
    EXPECT_EQ(var_info_read_back, var_info);
  }
}

TEST_F(VariableUtilsTest, WorkspaceInfoToFromAnyValue)
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

// TODO: add tests for failing parsing of AnyValue to VariableInfo/WorkspaceInfo