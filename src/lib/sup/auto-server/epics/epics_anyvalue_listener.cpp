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

#include <sup/auto-server/epics_anyvalue_listener.h>

#include <sup/auto-server/sup_auto_protocol.h>

#include <sup/epics/pv_access_client_pv.h>

#include <vector>

namespace
{

}  // unnamed namespace

namespace sup
{
namespace auto_server
{
class EPICSAnyValueListenerImpl
{
public:
  EPICSAnyValueListenerImpl(IAnyValueManager& av_mgr);
  ~EPICSAnyValueListenerImpl();

  void AddMonitorPV(const std::string& channel);

  void AddInputServer(const std::string& input_server_name);

private:
  void HandleUserInput(const std::string& input_server_name, const sup::dto::AnyValue& req_av);
  IAnyValueManager& m_av_mgr;
  std::vector<sup::epics::PvAccessClientPV> m_client_pvs;
};

EPICSAnyValueListener::EPICSAnyValueListener(const JobInfo& job_info, IAnyValueManager& av_mgr)
  : m_impl{new EPICSAnyValueListenerImpl(av_mgr)}
{
  auto value_set = GetValueSet(job_info);
  av_mgr.AddAnyValues(value_set);
  auto value_names = GetNames(value_set);
  for (const auto& value_name : value_names)
  {
    m_impl->AddMonitorPV(value_name);
  }
  auto input_server_name = GetInputServerName(job_info.GetPrefix());
  av_mgr.AddInputServer(input_server_name);
  m_impl->AddInputServer(input_server_name);
}

EPICSAnyValueListener::~EPICSAnyValueListener() = default;

IAnyValueManager::NameAnyValueSet EPICSAnyValueListener::GetValueSet(const JobInfo& job_info) const
{
  auto job_prefix = job_info.GetPrefix();
  auto n_vars = job_info.GetNumberOfVariables();
  auto initial_set = GetInitialValueSet(job_prefix, n_vars);
  auto n_instr = job_info.GetNumberOfInstructions();
  auto instr_set = GetInstructionValueSet(job_prefix, n_instr);
  for (const auto& instr_pair : instr_set)
  {
    initial_set.push_back(instr_pair);
  }
  return initial_set;
}

std::unique_ptr<IAnyValueListener> EPICSListenerFactoryFunction(
  const JobInfo& job_info, IAnyValueManager& av_mgr)
{
  return std::unique_ptr<IAnyValueListener>(new EPICSAnyValueListener(job_info, av_mgr));
}

EPICSAnyValueListenerImpl::EPICSAnyValueListenerImpl(IAnyValueManager& av_mgr)
  : m_av_mgr{av_mgr}
  , m_client_pvs{}
{}

EPICSAnyValueListenerImpl::~EPICSAnyValueListenerImpl() = default;

void EPICSAnyValueListenerImpl::AddMonitorPV(const std::string& channel)
{
  using sup::epics::PvAccessClientPV;
  auto cb = [this, channel](const PvAccessClientPV::ExtendedValue& ext_val) {
    if (ext_val.connected)
    {
      m_av_mgr.UpdateAnyValue(channel, ext_val.value);
    }
  };
  m_client_pvs.emplace_back(channel, cb);
}

void EPICSAnyValueListenerImpl::AddInputServer(const std::string& input_server_name)
{
  using sup::epics::PvAccessClientPV;
  // construct input server
  auto input_request_pv_name = GetInputRequestPVName(input_server_name);
  auto cb = [this, input_server_name](const PvAccessClientPV::ExtendedValue& ext_val) {
    if (ext_val.connected)
    {
      HandleUserInput(input_server_name, ext_val.value);
    }
  };
  m_client_pvs.emplace_back(input_request_pv_name, cb);
}

void EPICSAnyValueListenerImpl::HandleUserInput(const std::string& input_server_name,
                                                const sup::dto::AnyValue& req_av)
{
  auto req = DecodeInputRequest(req_av);
  if (!std::get<0>(req))
  {
    return;  // only react to active input requests
  }
  auto req_idx = std::get<1>(req);
  auto reply = m_av_mgr.GetUserInput(input_server_name, std::get<2>(req));
  // TODO: use rpc client to post this answer
}

}  // namespace auto_server

}  // namespace sup
