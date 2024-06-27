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

#ifndef SUP_AUTO_SERVER_INSTRUCTION_MAP_H_
#define SUP_AUTO_SERVER_INSTRUCTION_MAP_H_

#include <sup/dto/anyvalue.h>

#include <map>

namespace sup
{
namespace sequencer
{
class Instruction;
}  // namespace sequencer

namespace auto_server
{
/**
 * @brief InstructionMap builds a map from instruction pointers to a unique index. The use
 * of indexed identifiers instead of a single tree structure for all instructions limits network
 * traffic for single instruction updates.
 *
 * @note This class is immutable and builds all the necessary structures during construction. This
 * allows to use InstructionMap objects easily in a multithreaded context.
 */
class InstructionMap
{
public:
  using InstructionIndexMap = std::map<const sequencer::Instruction*, sup::dto::uint32>;
  /**
   * @brief Construct a InstructionMap object from the given root instruction.
   *
   * @details The constructor ensures that the mapping is between unique Instruction pointers, which
   * are not nullptr, and indices from zero to N-1, where N is the number of Instructions in the
   * provided Instruction tree.
   *
   * @param root Root of an instruction tree.
   */
  explicit InstructionMap(const sequencer::Instruction* root);

  /**
   * @brief Destructor.
   */
  ~InstructionMap();

  /**
   * @brief Find the instruction index corresponding to the given instruction.
   *
   * @param instr Instruction pointer.
   * @return Instruction index.
   *
   * @throw InvalidOperationException when the instruction is not known.
   */
  sup::dto::uint32 FindInstructionIndex(const sequencer::Instruction* instr) const;

  /**
   * @brief Return the map of instructions to their indices.
   *
   * @return map of instructions to indices.
   */
  const InstructionIndexMap& GetInstructionMapping() const;

  /**
   * @brief Return the number of instructions that are mapped.
   *
   * @return Number of instructions mapped.
   */
  sup::dto::uint32 GetNumberOfInstructions() const;

private:
  /**
   * @brief Initialization member function, called only during construction.
   *
   * @param root Root of the instruction tree whose nodes will be mapped.
   */
  void InitializeMap(const sequencer::Instruction* root);
  InstructionIndexMap m_instruction_map;
};

/**
 * @brief Build a vector of Instruction pointers in the order of the indices in the given map.
 * It is assumed that all indices in the map are distinct and span exactly the range from zero
 * to the size of the map: [0, size-1]. Nullptr entries are also not allowed.
 *
 * @param instr_map Map of instruction pointers to indices.
 * @return Ordered list of instruction pointers.
 *
 * @throw InvalidOperationException when the assumptions on the map are invalid.
 */
std::vector<const sequencer::Instruction*> GetReverseMap(
  const InstructionMap::InstructionIndexMap& instr_map);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INSTRUCTION_MAP_H_
