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

#include <app/oac-tree-server/utils.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree_server;

class AppUtilsTest : public ::testing::Test
{
protected:
  AppUtilsTest() = default;
  virtual ~AppUtilsTest() = default;
};

TEST_F(AppUtilsTest, Filelist)
{
  sup::cli::CommandLineParser parser;
  parser.AddOption({"-d", "--dir"}, "Directory containing files with xml extension to be parsed and run")
      .SetParameter(true)
      .SetValueName("directory_name");

  parser.AddPositionalOption("FILE...", "File(s) to be parsed and run as procedures");

  const int argc = 3;
  std::array<const char *, argc> argv{"progname", "file1", "file2"};

  EXPECT_TRUE(parser.Parse(argc, &argv[0]));
  std::vector<std::string> expected{ "file1", "file2" };
  EXPECT_EQ(utils::GetProcedureFilenames(parser), expected);
}
