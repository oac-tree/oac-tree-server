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

#include <sup/dto/basic_scalar_types.h>

#include <string>
#include <utility>
#include <vector>

namespace sup
{
namespace auto_server
{
using StringAttribute = std::pair<std::string, std::string>;

/**
 * @brief The VariableInfo structure represents the static information that a client can get about a
 * Workspace variable.
 */
struct VariableInfo
{
  std::string m_var_type;
  sup::dto::uint32 m_index;
  std::vector<StringAttribute> m_attributes;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_VARIABLE_INFO_H_
