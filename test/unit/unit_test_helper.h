/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - Sequencer
 *
 * Description   : Sequencer for operational procedures
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2023 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_AUTO_SERVER_UNIT_TEST_HELPER_H_
#define SUP_AUTO_SERVER_UNIT_TEST_HELPER_H_

#include <sup/auto-server/i_job_pv_server.h>

#include <string>

namespace sup
{
namespace auto_server
{
namespace UnitTestHelper
{

class CoutPVServer : public IJobPVServer
{
public:
  CoutPVServer();

  ~CoutPVServer();

  void UpdateInstructionTreePV(const sup::dto::AnyValue& tree_anyvalue) override;
};

class TemporaryTestFile
{
private:
  std::string m_filename;

public:
  TemporaryTestFile(std::string filename, std::string contents);
  ~TemporaryTestFile();
};

/**
 * Creates a string representing a valid XML of sequencer procedure by enclosing user provided body
 * between appropriate header and footer.
 */
std::string CreateProcedureString(const std::string& body);

}  // namespace UnitTestHelper

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_UNIT_TEST_HELPER_H_
