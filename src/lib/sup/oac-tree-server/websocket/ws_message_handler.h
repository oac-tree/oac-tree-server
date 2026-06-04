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

#ifndef SUP_OAC_TREE_SERVER_WS_MESSAGE_HANDLER_H_
#define SUP_OAC_TREE_SERVER_WS_MESSAGE_HANDLER_H_

#include <sup/oac-tree-server/base/message_reply_handler.h>

#include <sup/oac-tree-server/i_job_manager.h>

#include <sup/oac-tree/i_job_info_io.h>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief Parses JSON update messages and triggers the corresponding methods in IJobInfoIO.
 */
void HandleUpdateMessage(const std::string& msg, sup::oac_tree::IJobInfoIO& job_info_io);

/**
 * @brief Parses JSON messages intented for a job manager, delegate these to the appropriate methods
 * of the IJobManager and construct the corresponding JSON reply message.
 */
std::string HandleJobManagerMessage(const std::string& msg, IJobManager& job_manager);

/**
 * @brief Parses JSON reply messages from a remote job manager and delegates these to a
 * MessageReplyHandler if appropriate.
 */
void HandleJobManagerReply(const std::string& msg, MessageReplyHandler& reply_handler);

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_MESSAGE_HANDLER_H_
