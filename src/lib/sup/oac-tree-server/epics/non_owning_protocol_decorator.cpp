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

#include "non_owning_protocol_decorator.h"

namespace sup
{
namespace oac_tree_server
{

NonOwningProtocolDecorator::NonOwningProtocolDecorator(sup::protocol::Protocol& protocol)
  : m_protocol{protocol}
{}

NonOwningProtocolDecorator::~NonOwningProtocolDecorator() = default;

sup::protocol::ProtocolResult NonOwningProtocolDecorator::Invoke(const sup::dto::AnyValue& input,
                                                                 sup::dto::AnyValue& output)
{
  return m_protocol.Invoke(input, output);
}

sup::protocol::ProtocolResult NonOwningProtocolDecorator::Service(const sup::dto::AnyValue& input,
                                                                  sup::dto::AnyValue& output)
{
  return m_protocol.Service(input, output);
}

}  // namespace oac_tree_server

}  // namespace sup
