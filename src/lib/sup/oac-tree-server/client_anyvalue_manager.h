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

#ifndef SUP_OAC_TREE_SERVER_CLIENT_ANYVALUE_MANAGER_H_
#define SUP_OAC_TREE_SERVER_CLIENT_ANYVALUE_MANAGER_H_

#include <sup/oac-tree-server/i_anyvalue_manager.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/oac-tree/i_job_info_io.h>

#include <functional>
#include <map>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief ClientAnyValueManager
 */
class ClientAnyValueManager : public IAnyValueManager
{
public:
  using AnyValueCallback = std::function<void(sup::oac_tree::IJobInfoIO&, const sup::dto::AnyValue&)>;

  ClientAnyValueManager(sup::oac_tree::IJobInfoIO& job_info_io);

  virtual ~ClientAnyValueManager();

  bool AddAnyValues(const NameAnyValueSet& name_value_set) override;

  bool AddInputHandler(const std::string& input_server_name) override;

  bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) override;

  UserInputReply GetUserInput(const std::string& input_server_name, sup::dto::uint64 id,
                              const UserInputRequest& request) override;

  void Interrupt(const std::string& input_server_name, sup::dto::uint64 id) override;

private:
  sup::oac_tree::IJobInfoIO& m_job_info_io;
  std::map<std::string, AnyValueCallback> m_cb_map;
};

ClientAnyValueManager::AnyValueCallback CreateCallback(const ValueNameInfo& value_name_info);

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_CLIENT_ANYVALUE_MANAGER_H_
