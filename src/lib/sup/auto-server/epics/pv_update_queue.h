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

#ifndef SUP_AUTO_SERVER_PV_UPDATE_QUEUE_H_
#define SUP_AUTO_SERVER_PV_UPDATE_QUEUE_H_

#include "pv_update_command.h"

#include <sup/dto/anyvalue.h>

#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>

namespace sup
{

namespace auto_server
{

/**
 * @brief Threadsafe queue for PV update commands.
*/
class PVUpdateQueue
{
public:
  PVUpdateQueue();
  ~PVUpdateQueue();

  /**
   * @brief Push new PV update to queue.
   *
   * @param channel Channel to update.
   * @param value Value for update.
   */
  void Push(const std::string& channel, const sup::dto::AnyValue& value);

  /**
   * @brief Push a command that will terminate any processing loops.
   */
  void PushExit();

  /**
   * @brief Blocks until the queue becomes non-empty.
   */
  void WaitForNonEmpty();

  /**
   * @brief Pops out the whole queue.
   *
   * @note This allows the consumer of the queue to process all commands without needing to hold
   * any locks.
   */
  std::deque<PVUpdateCommand> PopCommands();

private:
  std::deque<PVUpdateCommand> m_pv_updates;
  mutable std::mutex m_mtx;
  std::condition_variable m_cv;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_PV_UPDATE_QUEUE_H_
