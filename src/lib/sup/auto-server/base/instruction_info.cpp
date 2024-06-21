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

#include <sup/auto-server/instruction_info.h>

#include <algorithm>

namespace sup
{
namespace auto_server
{

InstructionInfo::InstructionInfo(const std::string& instr_type, sup::dto::uint32 idx,
                                   std::vector<StringAttribute> attributes)
  : m_instr_type{instr_type}
  , m_index{idx}
  , m_attributes{std::move(attributes)}
  , m_children{}
{}

InstructionInfo::~InstructionInfo() = default;

InstructionInfo::InstructionInfo(InstructionInfo&& other) = default;
InstructionInfo& InstructionInfo::operator=(InstructionInfo&& other) = default;

std::string InstructionInfo::GetType() const
{
  return m_instr_type;
}

sup::dto::uint32 InstructionInfo::GetIndex() const
{
  return m_index;
}

std::vector<StringAttribute> InstructionInfo::GetAttributes() const
{
  return m_attributes;
}

InstructionInfo* InstructionInfo::AppendChild(std::unique_ptr<InstructionInfo> child)
{
  m_children.push_back(std::move(child));
  return m_children.back().get();
}

std::vector<InstructionInfo*> InstructionInfo::Children()
{
  std::vector<InstructionInfo*> result;
  auto func = [](const std::unique_ptr<InstructionInfo>& instr) {
    return instr.get();
  };
  std::transform(m_children.begin(), m_children.end(), std::back_inserter(result), func);
  return result;
}

std::vector<const InstructionInfo*> InstructionInfo::Children() const
{
  std::vector<const InstructionInfo*> result;
  auto func = [](const std::unique_ptr<InstructionInfo>& instr) {
    return instr.get();
  };
  std::transform(m_children.begin(), m_children.end(), std::back_inserter(result), func);
  return result;
}

bool operator==(const InstructionInfo& left, const InstructionInfo& right)
{
  if (left.GetType() != right.GetType())
  {
    return false;
  }
  if (left.GetIndex() != right.GetIndex())
  {
    return false;
  }
  if (left.GetAttributes() != right.GetAttributes())
  {
    return false;
  }
  auto left_children = left.Children();
  auto right_children = right.Children();
  if (left_children.size() != right_children.size())
  {
    return false;
  }
  // This recursive implementation is not very performant. However, it is currently only used
  // in unit testing.
  for (std::size_t i=0; i<left_children.size(); ++i)
  {
    if (*left_children[i] != *right_children[i])
    {
      return false;
    }
  }
  return true;
}

bool operator!=(const InstructionInfo& left, const InstructionInfo& right)
{
  return !(left == right);
}

}  // namespace auto_server

}  // namespace sup
