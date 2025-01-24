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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_OAC_TREE_SERVEREPICS_ANYVALUE_MANAGER_H_
#define SUP_OAC_TREE_SERVEREPICS_ANYVALUE_MANAGER_H_

#include <sup/oac-tree-server/i_anyvalue_manager.h>

#include <map>
#include <memory>
#include <mutex>

namespace sup
{
namespace auto_server
{
class EPICSServer;
class EPICSInputServer;

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
  bool AddInputHandler(const std::string& input_server_name) override;
  bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) override;
  UserInputReply GetUserInput(const std::string& input_server_name, sup::dto::uint64 id,
                              const UserInputRequest& request) override;
  void Interrupt(const std::string& input_server_name, sup::dto::uint64 id) override;

private:
  bool AddAnyValuesImpl(const NameAnyValueSet& name_value_set);
  bool ValidateNameValueSet(const NameAnyValueSet& name_value_set) const;
  EPICSServer* FindServer(const std::string& name) const;
  EPICSInputServer* FindInputServer(const std::string& server_name) const;

  mutable std::mutex m_map_mtx;
  mutable std::mutex m_user_input_mtx;
  std::map<std::string, EPICSServer*> m_name_server_map;
  std::vector<std::unique_ptr<EPICSServer>> m_servers;
  std::map<std::string, EPICSInputServer*> m_name_input_server_map;
  std::vector<std::unique_ptr<EPICSInputServer>> m_input_servers;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVEREPICS_ANYVALUE_MANAGER_H_
