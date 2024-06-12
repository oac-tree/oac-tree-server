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

#ifndef SUP_AUTO_SERVER_CLIENT_INTERFACE_MAPPER_H_
#define SUP_AUTO_SERVER_CLIENT_INTERFACE_MAPPER_H_

#include <sup/auto-server/job_info.h>

#include <sup/sequencer/job_interface.h>

#include <vector>

namespace sup
{
namespace auto_server
{

/**
 * @brief ClientInterfaceMapper is responsible for mapping instruction and variable updates
 * identified by indices to the corresponding member functions of a JobInterface, where these
 * are identified by Instruction pointers and Variable names.
 */
class ClientInterfaceMapper
{
public:
  ClientInterfaceMapper(sequencer::JobInterface& job_interface, const JobInfo& job_info);
  ~ClientInterfaceMapper();

  void InstructionUpdated(sup::dto::uint32 instr_idx, const sup::dto::AnyValue& value);
  void VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value);
private:
  sequencer::JobInterface& m_job_interface;
  // TODO: replace this anyvalue with a more direct object
  std::vector<sup::dto::AnyValue> m_instr_info;
  std::vector<std::string> m_var_name_map;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_CLIENT_INTERFACE_MAPPER_H_
