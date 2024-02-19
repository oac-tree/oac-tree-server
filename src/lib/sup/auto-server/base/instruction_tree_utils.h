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

#ifndef SUP_AUTO_SERVER_INSTRUCTION_TREE_UTILS_H_
#define SUP_AUTO_SERVER_INSTRUCTION_TREE_UTILS_H_

#include <map>
#include <set>
#include <string>

namespace sup
{
namespace sequencer
{
class Instruction;
}  // namespace sequencer

namespace auto_server
{
namespace utils
{

const std::string kChildrenField = "children";

std::string CreateUniquePath(const sequencer::Instruction* instruction,
                             const std::string& prefix,
                             std::set<std::string>& used_names);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INSTRUCTION_TREE_UTILS_H_
