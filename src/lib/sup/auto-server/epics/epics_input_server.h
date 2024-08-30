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

#ifndef SUP_AUTO_SERVER_EPICS_INPUT_SERVER_H_
#define SUP_AUTO_SERVER_EPICS_INPUT_SERVER_H_

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
 * @brief EPICSInputServer is the EPICS implementation of an RPC server that handles user input.
 */
class EPICSInputServer
{
public:
  explicit EPICSInputServer(const std::string& server_name);
  ~EPICSInputServer();

  // No copy or move
  EPICSInputServer(const EPICSInputServer& other) = delete;
  EPICSInputServer(EPICSInputServer&& other) = delete;
  EPICSInputServer& operator=(const EPICSInputServer& other) = delete;
  EPICSInputServer& operator=(EPICSInputServer&& other) = delete;

  /**
   * @brief Initiate a new request for user input.
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
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_INPUT_SERVER_H_
