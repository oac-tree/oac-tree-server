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

#include <sup/oac-tree-server/update_message_handler.h>

namespace sup
{
namespace oac_tree_server
{

UpdateMessageHandler::UpdateMessageHandler(sup::oac_tree::IJobInfoIO& job_info_io)
  : m_job_info_io{job_info_io}
{}

UpdateMessageHandler::~UpdateMessageHandler() = default;

void HandleMessage(const std::string& msg)
{
  (void)msg;
}

}  // namespace oac_tree_server

}  // namespace sup
