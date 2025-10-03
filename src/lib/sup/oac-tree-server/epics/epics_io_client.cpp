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
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "epics_io_client.h"

#include "epics_input_client.h"

#include <sup/oac-tree-server/i_anyvalue_manager.h>
#include <sup/oac-tree-server/client_reply_delegator.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/epics/pv_access_client_pv.h>
#include <sup/oac-tree/user_input_request.h>

#include <utility>
#include <vector>

namespace sup
{
namespace oac_tree_server
{
using sup::oac_tree::UserInputRequest;
using sup::oac_tree::InputRequestType;
using namespace std::placeholders;

class EPICSIOClientImpl
{
public:
  EPICSIOClientImpl(IAnyValueManager& av_mgr);
  ~EPICSIOClientImpl();

  bool AddAnyValues(const IAnyValueIO::NameAnyValueSet& monitor_set);

  bool AddInputHandler(const std::string& input_server_name);

private:
  void AddMonitorPV(const std::string& channel);

  void HandleUserInput(const std::string& input_server_name, const sup::dto::AnyValue& req_av);

  IAnyValueManager& m_av_mgr;
  std::unique_ptr<EPICSInputClient> m_input_client;
  std::unique_ptr<ClientReplyDelegator> m_reply_delegator;
  // Order matters: destroy these client PVs before the objects that are involved in callbacks:
  std::vector<sup::epics::PvAccessClientPV> m_client_pvs;
};

EPICSIOClient::EPICSIOClient(IAnyValueManager& av_mgr)
  : m_impl{std::make_unique<EPICSIOClientImpl>(av_mgr)}
{}

EPICSIOClient::~EPICSIOClient() = default;

bool EPICSIOClient::AddAnyValues(
  const IAnyValueIO::NameAnyValueSet& name_value_set)
{
  return m_impl->AddAnyValues(name_value_set);
}

bool EPICSIOClient::AddInputHandler(const std::string& input_server_name)
{
  return m_impl->AddInputHandler(input_server_name);
}

EPICSIOClientImpl::EPICSIOClientImpl(IAnyValueManager& av_mgr)
  : m_av_mgr{av_mgr}
  , m_input_client{}
  , m_reply_delegator{}
  , m_client_pvs{}
{}

EPICSIOClientImpl::~EPICSIOClientImpl() = default;

bool EPICSIOClientImpl::AddAnyValues(const IAnyValueIO::NameAnyValueSet& monitor_set)
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

bool EPICSIOClientImpl::AddInputHandler(const std::string& input_server_name)
{
  using sup::epics::PvAccessClientPV;
  if (!m_av_mgr.AddInputHandler(input_server_name))
  {
    return false;
  }
  m_input_client = std::make_unique<EPICSInputClient>(input_server_name);
  auto reply_func = std::bind(&EPICSInputClient::SetClientReply, m_input_client.get(), _1, _2);
  auto interrupt_func = std::bind(&IAnyValueManager::Interrupt, std::addressof(m_av_mgr),
                                  input_server_name, _1);
  m_reply_delegator = std::make_unique<ClientReplyDelegator>(reply_func, interrupt_func);
  auto input_request_pv_name = GetInputRequestPVName(input_server_name);
  IAnyValueIO::NameAnyValueSet input_pv_set;
  (void)input_pv_set.emplace_back(input_request_pv_name, kInputRequestAnyValue);
  auto cb = [this, input_server_name](const PvAccessClientPV::ExtendedValue& ext_val) {
    if (ext_val.connected)
    {
      auto [decoded, value] = Base64DecodeAnyValue(ext_val.value);
      if (decoded)
      {
        HandleUserInput(input_server_name, value);
      }
    }
  };
  (void)m_client_pvs.emplace_back(input_request_pv_name, cb);
  return true;
}

void EPICSIOClientImpl::AddMonitorPV(const std::string& channel)
{
  using sup::epics::PvAccessClientPV;
  auto cb = [this, channel](const PvAccessClientPV::ExtendedValue& ext_val) {
    if (ext_val.connected)
    {
      auto [decoded, value] = Base64DecodeAnyValue(ext_val.value);
      if (decoded)
      {
        m_av_mgr.UpdateAnyValue(channel, value);
      }
    }
  };
  (void)m_client_pvs.emplace_back(channel, cb);
}

void EPICSIOClientImpl::HandleUserInput(const std::string& input_server_name,
                                        const sup::dto::AnyValue& req_av)
{
  auto req = DecodeInputRequest(req_av);
  auto success = std::get<0>(req);
  auto id = std::get<1>(req);
  auto input_request = std::get<2>(req);
  if (!success || input_request.m_request_type == InputRequestType::kInvalid)
  {
    m_reply_delegator->InterruptAll();
    return;
  }
  auto reply = m_av_mgr.GetUserInput(input_server_name, id, input_request);
  m_reply_delegator->QueueReply(id, reply);
}

}  // namespace oac_tree_server

}  // namespace sup
