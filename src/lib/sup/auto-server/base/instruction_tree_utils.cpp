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

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/sequencer/instruction.h>

#include <algorithm>
#include <deque>

namespace sup
{
namespace auto_server
{
namespace utils
{

std::string CreateUniqueField(const sequencer::Instruction* instruction,
                              const std::vector<std::string>& used_names)
{
  auto instr_type = instruction->GetType();
  size_t index = 0;
  std::string suggestion = instr_type + std::to_string(index);
  auto iter = std::find(used_names.begin(), used_names.end(), suggestion);
  while (iter != used_names.end())
  {
    index++;
    suggestion = instr_type + std::to_string(index);
    iter = std::find(used_names.begin(), used_names.end(), suggestion);
  }
  return suggestion;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
