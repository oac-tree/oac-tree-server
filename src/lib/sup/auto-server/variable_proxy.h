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

#ifndef SUP_AUTO_SERVER_VARIABLE_PROXY_H_
#define SUP_AUTO_SERVER_VARIABLE_PROXY_H_

#include <sup/auto-server/variable_info.h>

namespace sup
{
namespace auto_server
{
/**
 * @brief The VariableProxy class represents a Variable at the client side. It contains the type of
 * Variable and its attributes. It is immutable, i.e. has no non-const member functions.
 *
 * @note This class is very similar to VariableInfo. However, since they serve different purposes,
 * they are kept as separate classes. This allows to easily extend one without affecting the other.
 */
class VariableProxy
{
public:
  explicit VariableProxy(const VariableInfo& var_info);
  ~VariableProxy();

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

#endif  // SUP_AUTO_SERVER_VARIABLE_PROXY_H_
