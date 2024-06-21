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

#include <sup/auto-server/instruction_info.h>
#include <sup/auto-server/job_value_mapper.h>

#include <sup/dto/anyvalue.h>

#include <memory>
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
 * @brief Create a InstructionInfo representation of an Instruction tree, providing all Instruction
 * types, their attributes and indices used for publishing their status.
 *
 * @param root Root instruction of the tree to represent.
 *
 * @return InstructionInfo tree representation.
 */
std::unique_ptr<InstructionInfo> CreateInstructionInfoTree(const sequencer::Instruction& root);

/**
 * @brief Convert the given AnyValue to an InstructionInfo tree.
 *
 * @param instr_info_anyvalue AnyValue representation of an instruction tree.
 * @return InstructionInfo tree.
 * @throw InvalidOperationException when the provided anyvalue has the wrong format.
 */
std::unique_ptr<InstructionInfo> ToInstructionInfoTree(
  const sup::dto::AnyValue& instr_info_anyvalue);

/**
 * @brief Convert the given InstructionInfo tree to an AnyValue.
 *
 * @param instr_info InstructionInfo tree.
 * @return AnyValue representation of the InstructionInfo tree.
 */
sup::dto::AnyValue ToAnyValueTree(const InstructionInfo& instr_info);

/**
 * @brief Create a InstructionInfo representation of a single Instruction, providing its type,
 * attributes and the given index. This function will not encode the Instruction's child
 * Instructions.
 *
 * @param instr Instruction to represent.
 * @param index Index to use.
 *
 * @return InstructionInfo representation.
 */
std::unique_ptr<InstructionInfo> CreateInstructionInfoNode(const sequencer::Instruction& instr,
                                                           sup::dto::uint32 index);

/**
 * @brief Convert the given AnyValue to a single InstructionInfo node. This function ignores any
 * possible child instructions.
 *
 * @param instr_info_anyvalue AnyValue representation of an instruction.
 * @return InstructionInfo node.
 * @throw InvalidOperationException when the provided anyvalue has the wrong format.
 */
std::unique_ptr<InstructionInfo> ToInstructionInfoNode(
  const sup::dto::AnyValue& instr_info_anyvalue);

/**
 * @brief Convert the given InstructionInfo node to an AnyValue. This function ignores any
 * possible child instructions.
 *
 * @param instr_info InstructionInfo node.
 * @return AnyValue representation of the InstructionInfo node.
 */
sup::dto::AnyValue ToAnyValueNode(const InstructionInfo& instr_info);

/**
 * @brief Create a unique member name for an Instruction to be used in a structured AnyValue.
 * An integer index is used to generate the member name to allow to easily deduce member order later.
 *
 * @param idx Index to use.
 * @return Unique member name.
 */
std::string CreateIndexedInstrChildName(std::size_t idx);

/**
 * @brief Validate if the given AnyValue has the right format to be parsed as an InstructionInfo
 * node.
 *
 * @details This function only checks the top level structure and does not validate any possible
 * children.
 *
 * @param instr_info AnyValue to validate.
 * @return true when the provided AnyValue can be correctly parsed to a InstructionInfo node.
 */
bool ValidateInstructionInfo(const sup::dto::AnyValue& instr_info);


// TODO: Remove the following functions

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
 * @param instr Instruction to represent.
 * @param index Index to put inside the node (referring to the served instruction AnyValue status).
 * @return AnyValue representation.
 * @throw InvalidOperationException when a nullptr is passed.
 */
sup::dto::AnyValue BuildInstructionInfoNode(const sequencer::Instruction* instr,
                                            sup::dto::uint32 index);

/**
 * @brief Convert the given AnyValue to a InstructionInfo object.
 *
 * @param instr_info_anyvalue AnyValue representation of an instruction.
 * @return New InstructionInfo object.
 * @throw InvalidOperationException when the provided anyvalue has the wrong format.
 */
std::unique_ptr<InstructionInfo> ToInstructionInfo(const sup::dto::AnyValue& instr_info_anyvalue);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INSTRUCTION_TREE_UTILS_H_
