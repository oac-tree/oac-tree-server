/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - AUTOMATION-SERVER
 *
 * Description   : SUP automation server
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

#ifndef SUP_AUTO_SERVER_JOB_PROXY_H_
#define SUP_AUTO_SERVER_JOB_PROXY_H_

#include <sup/auto-server/instruction_proxy.h>
#include <sup/auto-server/variable_proxy.h>

#include <sup/dto/anyvalue.h>

#include <memory>
#include <string>
#include <vector>

namespace sup
{
namespace auto_server
{
/**
 * @brief The JobProxy object represents all the static information about a Job on the client side.
 */
class JobProxy
{
public:
  JobProxy(const sup::dto::AnyValue& job_info_av);
  ~JobProxy();

  std::string GetPrefix() const;
  std::string GetProcedureName() const;
  std::size_t GetNumberOfVariables() const;
  std::size_t GetNumberOfInstructions() const;
  const std::vector<VariableInfo>& GetWorkspaceInfo() const;
  const InstructionProxy* GetRootInstructionInfo() const;
  std::vector<InstructionProxy*> GetInstructionIndexMap() const;

private:
  void InitializeWorkspaceInfo(const sup::dto::AnyValue& ws_info_av);
  void InitializeInstructionInfo(const sup::dto::AnyValue& instr_info_av, std::size_t n_instr);
  std::string m_job_prefix;
  std::string m_full_name;
  std::vector<VariableInfo> m_vars;
  std::unique_ptr<InstructionProxy> m_root;
  std::vector<InstructionProxy*> m_instr_map;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_PROXY_H_
