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

#include <sup/auto-server/instruction_proxy.h>

namespace sup
{
namespace auto_server
{

InstructionProxy::InstructionProxy(const InstructionInfo& instr_info)
  : m_instr_type{instr_info.m_instr_type}
  , m_index{instr_info.m_index}
  , m_attributes{instr_info.m_attributes}
{}

InstructionProxy::~InstructionProxy() = default;

std::string InstructionProxy::GetType() const
{
  return m_instr_type;
}

sup::dto::uint32 InstructionProxy::GetIndex() const
{
  return m_index;
}

std::vector<StringAttribute> InstructionProxy::GetAttributes() const
{
  return m_attributes;
}

}  // namespace auto_server

}  // namespace sup
