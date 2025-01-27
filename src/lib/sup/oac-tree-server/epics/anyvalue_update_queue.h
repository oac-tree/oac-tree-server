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
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_OAC_TREE_SERVER_ANYVALUE_UPDATE_QUEUE_H_
#define SUP_OAC_TREE_SERVER_ANYVALUE_UPDATE_QUEUE_H_

#include "anyvalue_update_command.h"

#include <sup/dto/anyvalue.h>

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <string>

namespace sup
{

namespace oac_tree_server
{

/**
 * @brief Threadsafe queue for AnyValue update commands.
*/
class AnyValueUpdateQueue
{
public:
  AnyValueUpdateQueue();
  ~AnyValueUpdateQueue();

  /**
   * @brief Push new PV update to queue.
   *
   * @param name Name of AnyValue to update.
   * @param value Value for update.
   */
  void Push(const std::string& name, const sup::dto::AnyValue& value);

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
  std::deque<AnyValueUpdateCommand> PopCommands();

private:
  std::deque<AnyValueUpdateCommand> m_value_updates;
  mutable std::mutex m_mtx;
  std::condition_variable m_cv;
};

using ValueUpdateFunction = std::function<void(const std::string&, const sup::dto::AnyValue&)>;
bool ProcessCommandQueue(std::deque<AnyValueUpdateCommand>& queue, const ValueUpdateFunction& func);

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_ANYVALUE_UPDATE_QUEUE_H_
