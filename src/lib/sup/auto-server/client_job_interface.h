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

#ifndef SUP_AUTO_SERVER_CLIENT_JOB_INTERFACE_H_
#define SUP_AUTO_SERVER_CLIENT_JOB_INTERFACE_H_

#include <sup/auto-server/job_info.h>

#include <sup/dto/anyvalue.h>
#include <sup/sequencer/job_states.h>

#include <string>
#include <vector>

namespace sup
{
namespace auto_server
{
class InstructionState;

class ClientJobInterface
{
public:
  virtual ~ClientJobInterface();

  virtual void InstructionUpdated(const InstructionInfo* instr_info,
                                  const InstructionState& state) = 0;
  virtual void VariableUpdated(const std::string& name, const sup::dto::AnyValue& value,
                               bool connected) = 0;
  virtual bool PutValue(const sup::dto::AnyValue& value, const std::string& description) = 0;
  virtual bool GetUserValue(sup::dto::AnyValue& value, const std::string& description) = 0;
  virtual int GetUserChoice(const std::vector<std::string>& options,
                            const sup::dto::AnyValue& metadata) = 0;
  virtual void Message(const std::string& message) = 0;
  virtual void Log(int severity, const std::string& message) = 0;
  virtual void OnStateChange(sequencer::JobState state) = 0;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_CLIENT_JOB_INTERFACE_H_
