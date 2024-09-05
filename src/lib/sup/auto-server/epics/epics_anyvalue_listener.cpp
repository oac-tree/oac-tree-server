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

#include "epics_input_client.h"

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

  bool AddAnyValueMonitors(const IAnyValueManager::NameAnyValueSet& monitor_set);

  bool AddInputClient(const std::string& input_server_name);

private:
  void AddMonitorPV(const std::string& channel);

  void HandleUserInput(const std::string& input_server_name, const sup::dto::AnyValue& req_av);

  IAnyValueManager& m_av_mgr;
  std::vector<sup::epics::PvAccessClientPV> m_client_pvs;
  std::unique_ptr<EPICSInputClient> m_input_client;
};

EPICSAnyValueListener::EPICSAnyValueListener(IAnyValueManager& av_mgr)
  : m_impl{new EPICSAnyValueListenerImpl(av_mgr)}
{}

EPICSAnyValueListener::~EPICSAnyValueListener() = default;

bool EPICSAnyValueListener::AddAnyValueMonitors(
  const IAnyValueManager::NameAnyValueSet& monitor_set)
{
  return m_impl->AddAnyValueMonitors(monitor_set);
}

bool EPICSAnyValueListener::AddInputClient(const std::string& input_server_name)
{
  return m_impl->AddInputClient(input_server_name);
}

std::unique_ptr<IAnyValueListener> EPICSListenerFactoryFunction(IAnyValueManager& av_mgr)
{
  return std::unique_ptr<IAnyValueListener>(new EPICSAnyValueListener(av_mgr));
}

EPICSAnyValueListenerImpl::EPICSAnyValueListenerImpl(IAnyValueManager& av_mgr)
  : m_av_mgr{av_mgr}
  , m_client_pvs{}
  , m_input_client{}
{}

EPICSAnyValueListenerImpl::~EPICSAnyValueListenerImpl() = default;

bool EPICSAnyValueListenerImpl::AddAnyValueMonitors(
  const IAnyValueManager::NameAnyValueSet& monitor_set)
{
  if (!m_av_mgr.AddAnyValues(monitor_set))
  {
    return false;
  }
  auto value_names = GetNames(monitor_set);
  for (const auto& value_name : value_names)
  {
    AddMonitorPV(value_name);
  }
  return true;
}

bool EPICSAnyValueListenerImpl::AddInputClient(const std::string& input_server_name)
{
  using sup::epics::PvAccessClientPV;
  if (!m_av_mgr.AddInputServer(input_server_name))
  {
    return false;
  }
  m_input_client = std::unique_ptr<EPICSInputClient>(new EPICSInputClient{input_server_name});
  auto input_request_pv_name = GetInputRequestPVName(input_server_name);
  IAnyValueManager::NameAnyValueSet input_pv_set;
  input_pv_set.emplace_back(input_request_pv_name, kInputRequestAnyValue);
  if (!m_av_mgr.AddAnyValues(input_pv_set))
  {
    return false;
  }
  auto cb = [this, input_server_name](const PvAccessClientPV::ExtendedValue& ext_val) {
    if (ext_val.connected)
    {
      HandleUserInput(input_server_name, ext_val.value);
    }
  };
  m_client_pvs.emplace_back(input_request_pv_name, cb);
  return true;
}

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
  (void)m_input_client->SetClientReply(req_idx, reply);
}

}  // namespace auto_server

}  // namespace sup
