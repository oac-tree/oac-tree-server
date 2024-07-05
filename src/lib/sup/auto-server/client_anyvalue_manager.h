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

#include <sup/auto-server/automation_client_interface.h>
#include <sup/auto-server/i_anyvalue_manager.h>
#include <sup/auto-server/job_info_value_mapper.h>

#include <functional>

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
  ClientAnyValueManager(std::size_t job_idx, const JobInfo& job_info,
                        AutomationClientInterface& client_if);

  virtual ~ClientAnyValueManager();

  bool AddAnyValues(const NameAnyValueSet& name_value_set) override;

  bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) override;

private:
  using AnyValueCallback = std::function<void(const sup::dto::AnyValue&)>;
  AnyValueCallback GetInstructionUpdateFunction(const std::string& val_name);
  std::size_t m_job_idx;
  JobInfoValueMapper m_value_mapper;
  AutomationClientInterface& m_client_if;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_CLIENT_ANYVALUE_MANAGER_H_
