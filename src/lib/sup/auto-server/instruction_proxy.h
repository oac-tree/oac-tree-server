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

#ifndef SUP_AUTO_SERVER_INSTRUCTION_PROXY_H_
#define SUP_AUTO_SERVER_INSTRUCTION_PROXY_H_

#include <sup/auto-server/instruction_info.h>

#include <memory>

namespace sup
{
namespace auto_server
{
/**
 * @brief The InstructionProxy class represents an Instruction at the client side. It contains the
 * type of Instruction, its index and its attributes. It does not contain dynamic state information,
 * such as its current execution status.
 *
 * @note This class is very similar to InstructionInfo. However, since they serve different
 * purposes, they are kept as separate classes. This allows to easily extend one without affecting
 * the other.
 */
class InstructionProxy
{
public:
  explicit InstructionProxy(const InstructionInfo& instr_info);
  ~InstructionProxy();

  std::string GetType() const;
  sup::dto::uint32 GetIndex() const;
  std::vector<StringAttribute> GetAttributes() const;

  void AppendChild(std::unique_ptr<InstructionProxy> child);

  std::vector<InstructionProxy*> Children();
  std::vector<const InstructionProxy*> Children() const;

private:
  std::string m_instr_type;
  sup::dto::uint32 m_index;
  std::vector<StringAttribute> m_attributes;
  std::vector<std::unique_ptr<InstructionProxy>> m_children;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INSTRUCTION_PROXY_H_
