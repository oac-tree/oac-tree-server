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

#include "instruction_tree_utils.h"

#include <sup/sequencer/instruction.h>

#include <deque>

namespace sup
{
namespace auto_server
{
namespace utils
{

std::string CreateFullInstructionPath(const std::string& prefix, const std::string& instr_name)
{
  auto full_prefix = prefix.empty() ? kChildrenField + "."
                                    : prefix + "." + kChildrenField + ".";
  return full_prefix + instr_name;
}

std::string CreateUniqueField(const sequencer::Instruction* instruction,
                             const std::string& prefix,
                             std::set<std::string>& used_names)
{
  auto instr_type = instruction->GetType();
  size_t index = 0;
  std::string suggestion = instr_type + std::to_string(index);
  auto iter = used_names.find(CreateFullInstructionPath(prefix, suggestion));
  while (iter != used_names.end())
  {
    index++;
    suggestion = instr_type + std::to_string(index);
    iter = used_names.find(CreateFullInstructionPath(prefix, suggestion));
  }
  used_names.insert(CreateFullInstructionPath(prefix, suggestion));
  return suggestion;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
