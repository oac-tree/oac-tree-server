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
/**
 * @brief InstructionTreeCache builds an AnyValue representing a full instruction tree and
 * the status of every instruction, which will be used to communicate instruction statuses over
 * the network. It also builds a map that allows to find the correct part into this AnyValue from
 * a given instruction pointer.
 *
 * @note This class is immutable and builds all the necessary structures during construction. This
 * allows to use InstructionTreeCache objects easily in a multithreaded context.
 */
class InstructionTreeCache
{
public:
  /**
   * @brief Construct a new InstructionTreeCache object from the given root of an instruction tree.
   *
   * @param root_instruction Root of the instruction tree to parse.
   */
  explicit InstructionTreeCache(const sequencer::Instruction* root_instruction);

  /**
   * @brief Destructor.
   */
  ~InstructionTreeCache();

  /**
   * @brief Find the path into the AnyValue corresponding to a given instruction pointer.
   *
   * @param instruction Instruction pointer to find.
   * @return Path into AnyValue corresponding to the given instruction pointer.
   *
   * @throw InvalidOperationException when instruction pointer is not known.
   */
  std::string FindInstructionPath(const sequencer::Instruction* instruction) const;

  /**
   * @brief Return the map of instruction pointers to paths.
   *
   * @return map of instruction pointers to paths.
   */
  std::map<const sequencer::Instruction*, std::string> GetInstructionPaths() const;

  /**
   * @brief Get the AnyValue representing the instruction tree in its initial state, i.e. without
   * breakpoints and all instructions in ExecutionStatus::NOT_STARTED.
   *
   * @return AnyValue representation of the instruction tree.
   */
  dto::AnyValue GetInitialInstructionTreeAnyValue() const;

private:
  /**
   * @brief Initialization member function, called only during construction.
   *
   * @param root_instruction Root of the instruction tree.
   */
  void InitializeCache(const sequencer::Instruction* root_instruction);
  std::map<const sequencer::Instruction*, std::string> m_instruction_paths;
  dto::AnyValue m_instr_tree_anyvalue;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INSTRUCTION_TREE_CACHE_H_
