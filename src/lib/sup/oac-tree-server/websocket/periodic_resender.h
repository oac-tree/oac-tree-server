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

#ifndef SUP_OAC_TREE_SERVER_WS_PERIODIC_RESENDER_H_
#define SUP_OAC_TREE_SERVER_WS_PERIODIC_RESENDER_H_

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief Periodically invokes a callback on its own thread until destroyed.
 *
 * @details This is used to re-publish an outstanding user input request to all websocket clients,
 * so that clients connecting after the initial broadcast still receive the request. The callback is
 * first invoked only after one interval has elapsed (the initial publish is the caller's
 * responsibility). On destruction the resender stops and joins its thread; the interruptible wait
 * ensures it stops promptly without waiting for a full interval.
 */
class PeriodicResender
{
public:
  PeriodicResender(std::function<void()> publish, std::chrono::milliseconds interval);
  ~PeriodicResender();

  // No copy or move
  PeriodicResender(const PeriodicResender& other) = delete;
  PeriodicResender(PeriodicResender&& other) = delete;
  PeriodicResender& operator=(const PeriodicResender& other) = delete;
  PeriodicResender& operator=(PeriodicResender&& other) = delete;

private:
  void Run();

  std::function<void()> m_publish;
  std::chrono::milliseconds m_interval;
  std::mutex m_mtx;
  std::condition_variable m_cv;
  bool m_stop;
  std::thread m_thread;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_PERIODIC_RESENDER_H_
