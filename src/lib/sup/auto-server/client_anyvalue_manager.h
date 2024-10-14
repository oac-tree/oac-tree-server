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

#ifndef SUP_AUTO_SERVER_CLIENT_ANYVALUE_MANAGER_H_
#define SUP_AUTO_SERVER_CLIENT_ANYVALUE_MANAGER_H_

#include <sup/auto-server/i_anyvalue_manager.h>
#include <sup/auto-server/i_job_info_io.h>
#include <sup/auto-server/sup_auto_protocol.h>

#include <functional>
#include <map>

namespace sup
{
namespace auto_server
{

/**
 * @brief ClientAnyValueManager
 */
class ClientAnyValueManager : public IAnyValueManager
{
public:
  using AnyValueCallback = std::function<void(IJobInfoIO&, const sup::dto::AnyValue&)>;

  ClientAnyValueManager(IJobInfoIO& job_info_io);

  virtual ~ClientAnyValueManager();

  bool AddAnyValues(const NameAnyValueSet& name_value_set) override;

  bool AddInputHandler(const std::string& input_server_name) override;

  bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) override;

  sup::dto::AnyValue GetUserInput(const std::string& input_server_name,
                                  const AnyValueInputRequest& request) override;

private:
  IJobInfoIO& m_job_info_io;
  std::map<std::string, AnyValueCallback> m_cb_map;
};

ClientAnyValueManager::AnyValueCallback CreateCallback(const ValueNameInfo& value_name_info);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_CLIENT_ANYVALUE_MANAGER_H_
