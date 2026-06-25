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

#include <sup/oac-tree-server/base/input_request_server.h>
#include <sup/oac-tree-server/base/message_reply_handler.h>
#include <sup/oac-tree-server/websocket/ws_message.h>

#include <sup/oac-tree-server/i_job_manager.h>

#include <sup/oac-tree/i_job_info_io.h>

#include <sup/dto/anyvalue.h>

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief Parses JSON update messages and triggers the corresponding methods in IJobInfoIO.
 */
void HandleUpdateMessage(const std::string& msg, sup::oac_tree::IJobInfoIO& job_info_io);

/**
 * @brief Callbacks invoked by HandleClientJobMessage for the user input messages that a client
 * receives on a job endpoint. Each is optional (may be empty).
 */
struct ClientInputCallbacks
{
  std::function<void(sup::dto::uint64 id, const sup::dto::AnyValue& value,
                     const std::string& description)> on_value_request{};
  std::function<void(sup::dto::uint64 id, const std::vector<std::string>& options,
                     const sup::dto::AnyValue& metadata)> on_choice_request{};
  std::function<void(sup::dto::uint64 id)> on_cancelled{};
};

/**
 * @brief Parses a JSON message received by a client on a job endpoint. User input request messages
 * (get user value / get user choice) and user input cancellation are dispatched to the matching
 * callback; all other (one-way) update messages are forwarded to the IJobInfoIO. Malformed messages
 * are silently ignored.
 */
void HandleClientJobMessage(const std::string& msg, sup::oac_tree::IJobInfoIO& job_info_io,
                            const ClientInputCallbacks& callbacks);

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

/**
 * @brief Parses a JSON user input reply message (get user value / get user choice) and forwards the
 * reply to the given InputRequestServer. Malformed messages are silently ignored.
 */
void HandleUserInputReply(const std::string& msg, InputRequestServer& request_server);

/**
 * @brief Parses the JSON reply message for the number of jobs and returns the value.
 */
std::optional<sup::dto::uint32> ParseNumberOfJobsReply(const std::string& reply);

/**
 * @brief Parses the JSON reply message for job information and returns the value.
 */
std::optional<sup::oac_tree::JobInfo> ParseJobInfoReply(const std::string& reply);

/**
 * @brief Parses a JSON job snapshot message (update type kJobSnapshot) into a JobSnapshot. Returns
 * nullopt if the message is malformed.
 */
std::optional<JobSnapshot> ParseJobSnapshotMessage(const std::string& msg);

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_MESSAGE_HANDLER_H_
