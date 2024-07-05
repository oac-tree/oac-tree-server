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

#ifndef SUP_AUTO_SERVER_JOB_INFO_VALUE_MAPPER_H_
#define SUP_AUTO_SERVER_JOB_INFO_VALUE_MAPPER_H_

#include <sup/auto-server/job_info.h>

#include <map>
#include <string>
#include <vector>

namespace sup
{
namespace auto_server
{
/**
 * @brief JobInfoValueMapper is responsible for the mapping of AnyValue names to variable names and
 * InstructionInfo pointers.
 */
class JobInfoValueMapper
{
public:
  explicit JobInfoValueMapper(const JobInfo& job_info);
  ~JobInfoValueMapper();

  std::vector<std::string> GetInstructionValueNames() const;

  const InstructionInfo* FindInstructionInfo(const std::string& val_name) const;

  std::vector<std::string> GetVariableValueNames() const;

  std::string FindVariableName(const std::string& val_name) const;

private:
  void InitInstructionMap(const JobInfo& job_info);
  void InitVariableMap(const JobInfo& job_info);
  std::map<std::string, const InstructionInfo*> m_instr_map;
  std::map<std::string, std::string> m_var_map;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_INFO_VALUE_MAPPER_H_
