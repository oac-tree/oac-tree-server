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

#include "pv_update_queue.h"

namespace sup
{
namespace auto_server
{

PVUpdateQueue::PVUpdateQueue()
  : m_pv_updates{}
  , m_mtx{}
  , m_cv{}
{}

PVUpdateQueue::~PVUpdateQueue()
{}

void PVUpdateQueue::Push(const std::string& channel, const sup::dto::AnyValue& value)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_pv_updates.emplace_back(channel, value);
  }
  m_cv.notify_one();
}

void PVUpdateQueue::PushExit()
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_pv_updates.emplace_back(PVUpdateCommand::ExitTag{});
  }
  m_cv.notify_one();
}

void PVUpdateQueue::WaitForNonEmpty()
{
  std::unique_lock<std::mutex> lk{m_mtx};
  auto pred = [this]{
    return !m_pv_updates.empty();
  };
  m_cv.wait(lk, pred);
}

std::deque<PVUpdateCommand> PVUpdateQueue::PopCommands()
{
  std::deque<PVUpdateCommand> result;
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_pv_updates.swap(result);
  }
  return result;
}

}  // namespace auto_server

}  // namespace sup
