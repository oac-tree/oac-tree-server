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

#ifndef SUP_AUTO_SERVER_EPICS_ANYVALUE_LISTENER_H_
#define SUP_AUTO_SERVER_EPICS_ANYVALUE_LISTENER_H_

#include <sup/auto-server/i_anyvalue_listener.h>

#include <memory>

namespace sup
{
namespace auto_server
{
class EPICSAnyValueListenerImpl;
/**
 * @brief EPICSAnyValueListener implements IAnyValueListener using EPICS PvAccess.
 */
class EPICSAnyValueListener : public IAnyValueListener
{
public:
  EPICSAnyValueListener(IAnyValueManager& av_mgr);
  ~EPICSAnyValueListener();

  bool AddAnyValueMonitors(const IAnyValueManager::NameAnyValueSet& monitor_set) override;

  bool AddInputClient(const std::string& input_server_name) override;

private:
  std::unique_ptr<EPICSAnyValueListenerImpl> m_impl;
};

std::unique_ptr<IAnyValueListener> EPICSListenerFactoryFunction(IAnyValueManager& av_mgr);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_ANYVALUE_LISTENER_H_
