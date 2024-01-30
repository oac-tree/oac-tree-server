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

#ifndef SUP_AUTO_SERVER_JOB_COMMAND_QUEUE_H_
#define SUP_AUTO_SERVER_JOB_COMMAND_QUEUE_H_

#include "job_commands.h"

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>

namespace sup
{
namespace auto_server
{
/**
 * @brief Threadsafe fifo queue for commands, used in `JobController`.
*/
class JobCommandQueue
{
public:
  JobCommandQueue();
  ~JobCommandQueue();

  /**
   * @brief Push new command to queue.
   *
   * @param command Command to push.
   */
  void Push(JobCommand command);

  /**
   * @brief Push new command to queue with priority: remove all commands from the queue first that
   * have lower or equal priority. If the queue can be completely emptied, execute the passed
   * function, push the given command and return true. Otherwise, return false immediately upon
   * encountering a command with higher priority.
   *
   * @param command Command to push.
   * @param func Function to execute if queue was successfully emptied.
   *
   * @return true when queue was completely emptied and only the new command was added afterwards.
   */
  bool PriorityPush(JobCommand command, std::function<void()> func);

  /**
   * @brief Query if command queue is empty.
   *
   * @return true when queue is empty.
   */
  bool IsEmpty() const;

  /**
   * @brief Try to pop a command from the queue.
   *
   * @param command Output parameter that will hold the popped command if successful.
   * @return true when successful. False indicates the queue was empty.
   */
  bool TryPop(JobCommand& command);

  /**
   * @brief Blocks until a command can be retrieved from the queue and returns that command.
   *
   * @return Popped command
   */
  JobCommand WaitForNextCommand();

private:
  std::deque<JobCommand> m_command_queue;
  mutable std::mutex m_mtx;
  std::condition_variable m_cv;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_COMMAND_QUEUE_H_
