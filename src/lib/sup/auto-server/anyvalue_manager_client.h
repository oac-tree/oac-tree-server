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

#ifndef SUP_AUTO_SERVER_ANYVALUE_MANAGER_CLIENT_H_
#define SUP_AUTO_SERVER_ANYVALUE_MANAGER_CLIENT_H_

#include <sup/auto-server/i_anyvalue_manager.h>
#include <sup/auto-server/client_interface_mapper.h>

namespace sup
{
namespace auto_server
{

/**
 * @brief AnyValueManagerClient is responsible for wiring updates of (name, value) pairs to the
 * appropriate methods of a JobInterface object.
 */
class AnyValueManagerClient : public IAnyValueManager
{
public:
  explicit AnyValueManagerClient(ClientInterfaceMapper& interface_mapper);
  virtual ~AnyValueManagerClient();

  /**
   * @brief On adding a new set of values, this class will establish the appropriate methods to
   * call during subsequent update calls.
   *
   * @param name_value_set List of pairs of names and AnyValues.
   * @return true when successful. In case of failure, none of the values is assumed to be added.
   */
  bool AddAnyValues(const NameAnyValueSet& name_value_set) override;

  bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) override;
private:
  ClientInterfaceMapper& m_interface_mapper;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_ANYVALUE_MANAGER_CLIENT_H_
