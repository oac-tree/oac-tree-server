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

#ifndef SUP_AUTO_SERVER_SERVER_JOB_INFO_IO_H_
#define SUP_AUTO_SERVER_SERVER_JOB_INFO_IO_H_

#include <sup/auto-server/i_job_info_io.h>

namespace sup
{
namespace auto_server
{
/**
 * @brief Implementation of IJobInfoIO that delegates its calls to an IAnyValueManager
 * implementation. This implementation will be used at the server side.
 */
class ServerJobInfoIO : public IJobInfoIO
{
public:
  ServerJobInfoIO(const std::string& job_prefix);
  virtual ~ServerJobInfoIO();

  void UpdateInstructionStatus(sup::dto::uint32 instr_idx,
                               sup::sequencer::ExecutionStatus status) override;

  void VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                       bool connected) override;

  bool PutValue(const sup::dto::AnyValue& value, const std::string& description) override;

  bool GetUserValue(sup::dto::AnyValue& value, const std::string& description) override;

  int GetUserChoice(const std::vector<std::string>& options,
                    const sup::dto::AnyValue& metadata) override;

  void Message(const std::string& message) override;

  void Log(int severity, const std::string& message) override;

  void OnStateChange(sup::sequencer::JobState state) override;

  void OnBreakpointChange(sup::dto::uint32 instr_idx, bool breakpoint_set) override;

private:
  const std::string m_job_prefix;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SERVER_JOB_INFO_IO_H_
