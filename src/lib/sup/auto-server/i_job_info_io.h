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

#ifndef SUP_AUTO_SERVER_I_JOB_INFO_IO_H_
#define SUP_AUTO_SERVER_I_JOB_INFO_IO_H_

#include <sup/dto/anyvalue.h>
#include <sup/sequencer/execution_status.h>
#include <sup/sequencer/job_states.h>

namespace sup
{
namespace auto_server
{
/**
 * @brief Abstract interface for runtime updates and user input/output of a job, represented by
 * a JobInfo.
 *
 */
class IJobInfoIO
{
public:
  virtual ~IJobInfoIO();

  virtual void UpdateInstructionStatus(sup::dto::uint32 instr_idx,
                                       sup::sequencer::ExecutionStatus status) = 0;

  virtual void VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                               bool connected) = 0;

  virtual bool PutValue(const sup::dto::AnyValue& value, const std::string& description) = 0;

  virtual bool GetUserValue(sup::dto::AnyValue& value, const std::string& description) = 0;

  virtual int GetUserChoice(const std::vector<std::string>& options,
                            const sup::dto::AnyValue& metadata) = 0;

  virtual void Message(const std::string& message) = 0;

  virtual void Log(int severity, const std::string& message) = 0;

  virtual void OnStateChange(sup::sequencer::JobState state) = 0;

  virtual void OnBreakpointChange(sup::dto::uint32 instr_idx,
                                  bool breakpoint_set) = 0;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_JOB_INFO_IO_H_
