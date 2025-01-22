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

#include <sup/auto-server/automation_client_stack.h>

#include <sup/auto-server/automation_protocol_client.h>
#include <sup/auto-server/exceptions.h>

#include <sup/protocol/protocol_rpc_client.h>

namespace sup
{
namespace auto_server
{

class AutomationClientStack::AutomationClientStackImpl
{
public:
  explicit AutomationClientStackImpl(std::unique_ptr<sup::protocol::Protocol> info_protocol,
                                     std::unique_ptr<sup::protocol::Protocol> control_protocol);
  ~AutomationClientStackImpl() = default;

  IJobManager& GetJobManager();

private:
  std::unique_ptr<sup::protocol::Protocol> m_info_protocol;
  std::unique_ptr<sup::protocol::Protocol> m_control_protocol;
  AutomationProtocolClient m_auto_protocol_client;
};

AutomationClientStack::AutomationClientStack(
  std::unique_ptr<sup::protocol::Protocol> info_protocol,
  std::unique_ptr<sup::protocol::Protocol> control_protocol)
  : m_impl{}
{
  if (!info_protocol || !control_protocol)
  {
    const std::string error =
      "AutomationClientStack ctor: trying to construct a client stack with an empty protocol";
    throw InvalidOperationException(error);
  }
  m_impl = std::make_unique<AutomationClientStackImpl>(std::move(info_protocol),
                                                       std::move(control_protocol));
}

AutomationClientStack::~AutomationClientStack() = default;

std::string AutomationClientStack::GetServerPrefix() const
{
  return m_impl->GetJobManager().GetServerPrefix();
}

sup::dto::uint32 AutomationClientStack::GetNumberOfJobs() const
{
  return m_impl->GetJobManager().GetNumberOfJobs();
}

sup::oac_tree::JobInfo AutomationClientStack::GetJobInfo(sup::dto::uint32 job_idx) const
{
  return m_impl->GetJobManager().GetJobInfo(job_idx);
}

void AutomationClientStack::EditBreakpoint(sup::dto::uint32 job_idx, sup::dto::uint32 instr_idx,
                                           bool breakpoint_active)
{
  return m_impl->GetJobManager().EditBreakpoint(job_idx, instr_idx, breakpoint_active);
}

void AutomationClientStack::SendJobCommand(sup::dto::uint32 job_idx,
                                           sup::oac_tree::JobCommand command)
{
  return m_impl->GetJobManager().SendJobCommand(job_idx, command);
}

AutomationClientStack::AutomationClientStackImpl::AutomationClientStackImpl(
  std::unique_ptr<sup::protocol::Protocol> info_protocol,
  std::unique_ptr<sup::protocol::Protocol> control_protocol)
  : m_info_protocol{std::move(info_protocol)}
  , m_control_protocol{std::move(control_protocol)}
  , m_auto_protocol_client{*m_info_protocol, *m_control_protocol}
{}

IJobManager& AutomationClientStack::AutomationClientStackImpl::GetJobManager()
{
  return m_auto_protocol_client;
}

}  // namespace auto_server

}  // namespace sup
