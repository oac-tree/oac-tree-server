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

#include "periodic_resender.h"

#include <utility>

namespace sup
{
namespace oac_tree_server
{

PeriodicResender::PeriodicResender(std::function<void()> publish,
                                   std::chrono::milliseconds interval)
  : m_publish{std::move(publish)}
  , m_interval{interval}
  , m_mtx{}
  , m_cv{}
  , m_stop{false}
  , m_thread{&PeriodicResender::Run, this}
{}

PeriodicResender::~PeriodicResender()
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_stop = true;
  }
  m_cv.notify_one();
  if (m_thread.joinable())
  {
    m_thread.join();
  }
}

void PeriodicResender::Run()
{
  std::unique_lock<std::mutex> lk{m_mtx};
  while (!m_stop)
  {
    if (m_cv.wait_for(lk, m_interval, [this]() { return m_stop; }))
    {
      break;  // Stop was requested before the interval elapsed.
    }
    // The interval elapsed without a stop request: re-publish without holding the lock.
    lk.unlock();
    m_publish();
    lk.lock();
  }
}

}  // namespace oac_tree_server

}  // namespace sup
