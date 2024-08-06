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
  // parse all required pv_names and add those to the AVMgr
  // then add them as PVs to the impl
}

EPICSAnyValueListener::~EPICSAnyValueListener() = default;

std::unique_ptr<IAnyValueListener> EPICSListenerFactoryFunction(
  const JobInfo& job_info, IAnyValueManager& av_mgr)
{
  return std::unique_ptr<IAnyValueListener>(new EPICSAnyValueListener(job_info, av_mgr));
}

EPICSAnyValueListenerImpl::EPICSAnyValueListenerImpl(IAnyValueManager& av_mgr)
  : m_av_mgr{av_mgr}
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
