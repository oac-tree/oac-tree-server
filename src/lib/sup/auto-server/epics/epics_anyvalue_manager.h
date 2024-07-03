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

#ifndef SUP_AUTO_SERVER_EPICS_ANYVALUE_MANAGER_H_
#define SUP_AUTO_SERVER_EPICS_ANYVALUE_MANAGER_H_

#include <sup/auto-server/i_anyvalue_manager.h>

#include <map>
#include <memory>
#include <mutex>

namespace sup
{
namespace auto_server
{
class EPICSServer;

/**
 * @brief EPICSAnyValueManager implements IAnyValueManager using EPICS PvAccess and publishes
 * the managed AnyValues over this protocol.
 */
class EPICSAnyValueManager : public IAnyValueManager
{
public:
  EPICSAnyValueManager();
  ~EPICSAnyValueManager();

  bool AddAnyValues(const NameAnyValueSet& name_value_set) override;
  bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) override;

private:
  bool ValidateNameValueSet(const NameAnyValueSet& name_value_set) const;
  EPICSServer* FindServer(const std::string& name) const;

  mutable std::mutex m_map_mutex;
  std::map<std::string, EPICSServer*> m_name_server_map;
  std::vector<std::unique_ptr<EPICSServer>> m_set_servers;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_ANYVALUE_MANAGER_H_
