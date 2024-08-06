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

#include "epics_anyvalue_listener.h"

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

private:
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
}

EPICSAnyValueListener::~EPICSAnyValueListener() = default;

IAnyValueManager::NameAnyValueSet EPICSAnyValueListener::GetValueSet(const JobInfo& job_info) const
{
  auto job_prefix = job_info.GetPrefix();
  IAnyValueManager::NameAnyValueSet result;
  auto job_value_name = GetJobStatePVName(job_prefix);
  auto job_value = GetJobStateValue(sequencer::JobState::kInitial);
  result.emplace_back(job_value_name, job_value);
  for (sup::dto::uint32 var_idx = 0; var_idx < job_info.GetNumberOfVariables(); ++var_idx)
  {
    const auto name = GetVariablePVName(job_prefix, var_idx);
    result.emplace_back(name, kVariableAnyValue);
  }
  // TODO: add instructions
  return result;
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

}  // namespace auto_server

}  // namespace sup
