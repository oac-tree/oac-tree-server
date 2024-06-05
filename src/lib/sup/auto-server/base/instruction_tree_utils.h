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

#include <sup/auto-server/job_value_mapper.h>

#include <sup/dto/anyvalue.h>

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

/**
 * @brief Build an AnyValue representation of the instruction tree, providing instruction types,
 * attributes and the instruction index used for publishing its status.
 *
 * @param root Root instruction of the tree to represent.
 * @param mapper JobValueMapper object that provides indices for each instruction.
 * @return AnyValue representation.
 */
sup::dto::AnyValue BuildInstructionTreeInfo(const sequencer::Instruction* root,
                                            const JobValueMapper& mapper);

/**
 * @brief Build an AnyValue representation of an instruction, providing its type,
 * attributes and the instruction index used for publishing its status.
 *
 * @param root Instruction to represent.
 * @param index Index to put inside the node (referring to the served instruction AnyValue status).
 * @return AnyValue representation.
 */
sup::dto::AnyValue BuildInstructionInfoNode(const sequencer::Instruction* instr,
                                            sup::dto::uint32 index);

/**
 * @brief Create a unique member name for a structured AnyValue. An integer index is used to
 * generate the member name to allow to easily deduce member order later.
 *
 * @param idx Index to use.
 * @return Unique member name.
 */
std::string CreateIndexedMemberName(std::size_t idx);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INSTRUCTION_TREE_UTILS_H_
