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

#include <sup/auto-server/instruction_state.h>
#include <sup/auto-server/job_info.h>

#include <sup/dto/anyvalue.h>
#include <sup/sequencer/job_states.h>

#include <string>
#include <vector>

namespace sup
{
namespace auto_server
{

class AutomationClientInterface
{
public:
  virtual ~AutomationClientInterface();

  virtual void InstructionUpdated(std::size_t job_idx, const InstructionInfo* instr_info,
                                  const InstructionState& state) = 0;
  virtual void VariableUpdated(std::size_t job_idx, const std::string& name,
                               const sup::dto::AnyValue& value, bool connected) = 0;
  virtual bool PutValue(std::size_t job_idx, const sup::dto::AnyValue& value,
                        const std::string& description) = 0;
  virtual bool GetUserValue(std::size_t job_idx, sup::dto::AnyValue& value,
                            const std::string& description) = 0;
  virtual int GetUserChoice(std::size_t job_idx, const std::vector<std::string>& options,
                            const sup::dto::AnyValue& metadata) = 0;
  virtual void Message(std::size_t job_idx, const std::string& message) = 0;
  virtual void Log(std::size_t job_idx, int severity, const std::string& message) = 0;
  virtual void OnStateChange(std::size_t job_idx, sequencer::JobState state) = 0;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_CLIENT_JOB_INTERFACE_H_
