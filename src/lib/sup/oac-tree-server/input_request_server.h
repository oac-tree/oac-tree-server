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

#ifndef SUP_OAC_TREE_SERVER_INPUT_REQUEST_SERVER_H_
#define SUP_OAC_TREE_SERVER_INPUT_REQUEST_SERVER_H_

#include <sup/oac-tree/user_input_reply.h>

#include <condition_variable>
#include <mutex>
#include <utility>

namespace sup
{
namespace oac_tree_server
{

using sup::oac_tree::UserInputReply;

/**
 * @brief InputRequestServer is a helper class that manages a single request for user input at
 * a time. It is intended to be used in a multithreaded context, e.g. the server is waiting for a
 * reply, while a client thread sets it.
 *
 * @note This class does not prevent users to instantiate new requests when an older one was still
 * not satisfied, i.e. replied to by a client. Users of this class are thus responsible for
 * handling such cases.
 */
class InputRequestServer
{
public:
  InputRequestServer();
  ~InputRequestServer();

  // No copy or move
  InputRequestServer(const InputRequestServer& other) = delete;
  InputRequestServer(InputRequestServer&& other) = delete;
  InputRequestServer& operator=(const InputRequestServer& other) = delete;
  InputRequestServer& operator=(InputRequestServer&& other) = delete;

  /**
   * @brief Clear the internal reply field. This also disables any SetClientReply() calls from being
   * taken into account.
   *
   * @param id Identification of the user input request.
   */
   void InitNewRequest(sup::dto::uint64 id);

  /**
   * @brief Set a client reply. If the index does not match the current index, this reply will be
   * ignored.
   *
   * @param id Unique index that identifies a specific request for user input.
   * @param reply Reply from the client.
   *
   * @return true when successful. Failures can be due to mismatch in the index or the failure to
   * parse the reply object correctly.
   */
  bool SetClientReply(sup::dto::uint64 id, const UserInputReply& reply);

  /**
   * @brief Wait for a client to provide user input or timeout.
   *
   * @param id Unique index that identifies a specific request for user input.
   *
   * @return Boolean indicating success of retrieving a value (true) or interrupt (false) and the
   * provided AnyValue (if true).
   */
  std::pair<bool, UserInputReply> WaitForReply(sup::dto::uint64 id);

  void Interrupt(sup::dto::uint64 id);

private:
  sup::dto::uint64 m_request_id;
  UserInputReply m_reply;
  bool m_interrupt;
  std::mutex m_mtx;
  std::condition_variable m_cv;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_INPUT_REQUEST_SERVER_H_
