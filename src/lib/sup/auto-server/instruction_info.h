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

#ifndef SUP_AUTO_SERVER_INSTRUCTION_INFO_H_
#define SUP_AUTO_SERVER_INSTRUCTION_INFO_H_

#include <sup/auto-server/attribute_info.h>

#include <sup/dto/basic_scalar_types.h>

#include <memory>
#include <string>
#include <vector>

namespace sup
{
namespace auto_server
{
/**
 * @brief The InstructionInfo class represents the static information of an Instruction, including
 * its type, attributes and child instructions. It also contains an index that allows to uniquely
 * identify the Instruction within a procedure.
 */
class InstructionInfo
{
public:
  InstructionInfo(const std::string& instr_type, sup::dto::uint32 idx,
                   std::vector<StringAttribute> attributes);
  ~InstructionInfo();

  // Default move constructor/assignment
  InstructionInfo(InstructionInfo&& other);
  InstructionInfo& operator=(InstructionInfo&& other);

  std::string GetType() const;
  sup::dto::uint32 GetIndex() const;
  std::vector<StringAttribute> GetAttributes() const;

  /**
   * @brief Append a child to the end of its list of children.
   *
   * @param child Child InstructionInfo to add.
   * @return Pointer to added child.
   */
  InstructionInfo* AppendChild(std::unique_ptr<InstructionInfo> child);

  std::vector<InstructionInfo*> Children();
  std::vector<const InstructionInfo*> Children() const;

private:
  std::string m_instr_type;
  sup::dto::uint32 m_index;
  std::vector<StringAttribute> m_attributes;
  std::vector<std::unique_ptr<InstructionInfo>> m_children;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INSTRUCTION_INFO_H_
