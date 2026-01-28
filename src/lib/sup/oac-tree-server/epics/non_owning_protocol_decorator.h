/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : oac-tree server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_OAC_TREE_SERVER_NON_OWNING_PROTOCOL_DECORATOR_H_
#define SUP_OAC_TREE_SERVER_NON_OWNING_PROTOCOL_DECORATOR_H_

#include <sup/protocol/protocol.h>

namespace sup
{

/**
 * @brief NonOwningProtocolDecorator is a Protocol decorator that does not own its decoratee. This
 * allows users to transfer ownership of this class without requiring to relinquish ownership to
 * the original Protocol object.
 */
namespace oac_tree_server
{
class NonOwningProtocolDecorator : public sup::protocol::Protocol
{
public:
  NonOwningProtocolDecorator(sup::protocol::Protocol& protocol);
  ~NonOwningProtocolDecorator() override;

  sup::protocol::ProtocolResult Invoke(const sup::dto::AnyValue& input,
                                       sup::dto::AnyValue& output) override;
  sup::protocol::ProtocolResult Service(const sup::dto::AnyValue& input,
                                        sup::dto::AnyValue& output) override;

private:
  sup::protocol::Protocol& m_protocol;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_NON_OWNING_PROTOCOL_DECORATOR_H_
