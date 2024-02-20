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

#include <vector>
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

const std::string kInstructionNodeType = "sup::instructionNode/v1.0";
const std::string kExecStatusField = "exec_status";
const std::string kBreakpointField = "breakpoint";
const std::string kChildrenField = "children";

/**
 * @brief Create a full path into an AnyValue from a prefix and a specific instruction name.
 *
 * @note Every instruction in the tree is represented by a structure, containing its status and
 * a member structure for possible children. The full path contains these intermediate fieldnames
 * for the 'children' structure.
 *
 * @param prefix Path to the parent instruction or empty if instruction was the root.
 * @param instr_name Unique instruction name within a given parent.
 * @return Full path from root to the instruction.
 */
std::string CreateFullInstructionPath(const std::string& prefix, const std::string& instr_name);

/**
 * @brief Create a unique fieldname for the given instruction. Uniqueness means that no other child
 * instruction of its parent has the same name. The name consists of the instruction's type and an
 * integer index.
 *
 * @param instruction Instruction to provide name for.
 * @param used_names Instruction names already in use in its parent instruction.
 * @return Unique fieldname.
 */
std::string CreateUniqueField(const sequencer::Instruction* instruction,
                              const std::vector<std::string>& used_names);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INSTRUCTION_TREE_UTILS_H_
