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

#ifndef SUP_AUTO_SERVER_VARIABLE_INFO_H_
#define SUP_AUTO_SERVER_VARIABLE_INFO_H_

#include <sup/auto-server/attribute_info.h>

#include <sup/dto/basic_scalar_types.h>

#include <string>
#include <vector>

namespace sup
{
namespace auto_server
{
/**
 * @brief The VariableInfo class represents the static information of a Variable, including its
 * type and attributes. It also contains an index that allows to uniquely identify the Variable
 * within a workspace.
 *
 * @todo Check if putting the index inside is really needed, since the index can be deduced from
 * the order of VariableInfo objects that is used.
 */
class VariableInfo
{
public:
  VariableInfo(const std::string& var_type, sup::dto::uint32 idx,
               std::vector<StringAttribute> attributes);
  ~VariableInfo();

  std::string GetType() const;
  sup::dto::uint32 GetIndex() const;
  std::vector<StringAttribute> GetAttributes() const;

private:
  std::string m_var_type;
  sup::dto::uint32 m_index;
  std::vector<StringAttribute> m_attributes;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_VARIABLE_INFO_H_
