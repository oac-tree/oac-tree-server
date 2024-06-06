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

#ifndef SUP_AUTO_SERVER_JOB_VALUE_MAPPER_H_
#define SUP_AUTO_SERVER_JOB_VALUE_MAPPER_H_

#include <sup/sequencer/procedure.h>

#include <memory>
#include <string>
#include <vector>

namespace sup
{
namespace auto_server
{
class VariableMap;
class InstructionMap;
/**
 * @brief JobValueMapper is responsible for the mapping of variable names, instructions and
 * job states to the name of a server AnyValue.
 */
class JobValueMapper
{
public:
  JobValueMapper(const std::string& prefix, const sequencer::Procedure& proc);
  ~JobValueMapper();

  void InitializeInstructionTree(const sequencer::Instruction* root);

  std::string GetJobStateName() const;

  sup::dto::uint32 GetNumberOfInstructions() const;

  sup::dto::uint32 GetInstructionIndex(const sequencer::Instruction* instr) const;

  std::string GetInstructionValueName(const sequencer::Instruction* instr) const;

  std::vector<const sequencer::Instruction*> GetOrderedInstructions() const;

  std::vector<std::string> GetInstructionValueNames() const;

  sup::dto::uint32 GetNumberOfVariables() const;

  sup::dto::uint32 GetVariableIndex(const std::string& var_name) const;

  std::string GetVariableValueName(const std::string& var_name) const;

  std::vector<std::string> GetVariableValueNames() const;
private:
  const std::string m_prefix;
  const std::unique_ptr<VariableMap> m_variable_map;
  std::unique_ptr<InstructionMap> m_instruction_map;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_VALUE_MAPPER_H_
