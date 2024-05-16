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

#ifndef SUP_AUTO_SERVER_EPICS_SERVER_H_
#define SUP_AUTO_SERVER_EPICS_SERVER_H_

#include "anyvalue_update_queue.h"

#include <sup/auto-server/server_interface.h>

#include <future>

namespace sup
{
namespace auto_server
{

/**
 * @brief EPICSServer serves a set of PvAccess variables. The corresponding PVs are created during
 * construction and torn down upon destruction.
 */
class EPICSServer
{
public:
  EPICSServer(const ServerInterface::NameAnyValueSet& name_value_set);
  ~EPICSServer();

  void UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value);

private:
  void UpdateLoop(const ServerInterface::NameAnyValueSet& name_value_set);
  AnyValueUpdateQueue m_update_queue;
  std::future<void> m_update_future;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_SERVER_H_
