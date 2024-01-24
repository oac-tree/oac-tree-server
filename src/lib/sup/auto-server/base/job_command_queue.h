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
  bool TryPop(JobCommand command);

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
