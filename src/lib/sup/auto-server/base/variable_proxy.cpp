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

#include <sup/auto-server/variable_proxy.h>

#include "variable_utils.h"

namespace sup
{
namespace auto_server
{

VariableProxy::VariableProxy(const sup::dto::AnyValue& var_info)
  : m_var_type{}
  , m_index{}
  , m_attributes{}
{}

VariableProxy::~VariableProxy() = default;

std::string VariableProxy::GetType() const
{
  return m_var_type;
}

sup::dto::uint32 VariableProxy::GetIndex() const
{
  return m_index;
}

std::vector<StringAttribute> VariableProxy::GetAttributes() const
{
  return m_attributes;
}

}  // namespace auto_server

}  // namespace sup
