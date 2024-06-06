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

#include "anyvalue_update_queue.h"

namespace sup
{
namespace auto_server
{

AnyValueUpdateQueue::AnyValueUpdateQueue()
  : m_value_updates{}
  , m_mtx{}
  , m_cv{}
{}

AnyValueUpdateQueue::~AnyValueUpdateQueue()
{}

void AnyValueUpdateQueue::Push(const std::string& channel, const sup::dto::AnyValue& value)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_value_updates.push_back(AnyValueUpdateCommand::CreateValueUpdate(channel, value));
  }
  m_cv.notify_one();
}

void AnyValueUpdateQueue::PushExit()
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_value_updates.push_back(AnyValueUpdateCommand::CreateExitCommand());
  }
  m_cv.notify_one();
}

void AnyValueUpdateQueue::WaitForNonEmpty()
{
  std::unique_lock<std::mutex> lk{m_mtx};
  auto pred = [this]{
    return !m_value_updates.empty();
  };
  m_cv.wait(lk, pred);
}

std::deque<AnyValueUpdateCommand> AnyValueUpdateQueue::PopCommands()
{
  std::deque<AnyValueUpdateCommand> result;
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_value_updates.swap(result);
  }
  return result;
}

bool ProcessCommandQueue(std::deque<AnyValueUpdateCommand>& queue, const ValueUpdateFunction& func)
{
  while (!queue.empty())
  {
    auto& command = queue.front();
    if (command.GetCommandType() == AnyValueUpdateCommand::kExit)
    {
      queue.pop_front();
      return true;  // stop processing
    }
    func(command.Name(), command.Value());
    queue.pop_front();
  }
  return false;
}

}  // namespace auto_server

}  // namespace sup
