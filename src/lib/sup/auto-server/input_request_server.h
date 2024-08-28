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

#ifndef SUP_AUTO_SERVER_INPUT_REQUEST_SERVER_H_
#define SUP_AUTO_SERVER_INPUT_REQUEST_SERVER_H_

#include <sup/dto/anyvalue.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

namespace sup
{
namespace auto_server
{

/**
 * @brief InputRequestServer is a helper class that manages a single request for user input at
 * a time.
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
   * @brief Set a client reply. If the index does not match the current index, this reply will be
   * ignored.
   *
   * @param req_idx Unique index that identifies a specific request for user input.
   * @param reply AnyValue reply from the client.
   *
   * @return true when successful. Failures can be due to mismatch in the index or the failure to
   * parse the reply object correctly.
   */
  bool SetClientReply(sup::dto::uint64 req_idx, const sup::dto::AnyValue& reply);

  /**
   * @brief Clear the internal reply field. This also disables any client requests from being
   * taken into account.
   *
   * @return Unique index that identifies the current request. Client replies that do not
   * provide a matching index will be ignored.
   */
  sup::dto::uint64 InitNewRequest();

  /**
   * @brief Wait for a client to provide user input or timeout.
   *
   * @param req_idx Unique index that identifies a specific request for user input.
   * @param timeout_sec Timeout in seconds.
   *
   * @return Boolean indicating success of retrieving a value (true) or timeout (false) and the
   * provided AnyValue (if true).
   */
  std::pair<bool, sup::dto::AnyValue> WaitForReply(sup::dto::uint64 req_idx, double timeout_sec);

private:
  sup::dto::uint64 m_request_idx;
  sup::dto::AnyValue m_reply;
  std::mutex m_mtx;
  std::condition_variable m_cv;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INPUT_REQUEST_SERVER_H_
