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

#ifndef SUP_AUTO_SERVER_INSTRUCTION_TREE_CACHE_H_
#define SUP_AUTO_SERVER_INSTRUCTION_TREE_CACHE_H_

#include <sup/dto/anyvalue.h>

#include <map>
#include <string>

namespace sup
{
namespace sequencer
{
class Instruction;
}  // namespace sequencer

namespace auto_server
{
class InstructionTreeCache
{
public:
  explicit InstructionTreeCache(const sequencer::Instruction* root_instruction);
  ~InstructionTreeCache();

  std::string FindInstructionPath(const sequencer::Instruction* instruction) const;

  std::map<const sequencer::Instruction*, std::string> GetInstructionPaths() const;

  dto::AnyValue GetInitialInstructionTreeAnyValue() const;
private:
  void InitializeCache(const sequencer::Instruction* root_instruction);
  std::map<const sequencer::Instruction*, std::string> m_instruction_paths;
  dto::AnyValue m_proc_anyvalue;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INSTRUCTION_TREE_CACHE_H_
