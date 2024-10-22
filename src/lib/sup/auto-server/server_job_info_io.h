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

#include <sup/auto-server/i_anyvalue_manager.h>
#include <sup/auto-server/index_generator.h>

#include <sup/sequencer/i_job_info_io.h>

namespace sup
{
namespace auto_server
{
/**
 * @brief Implementation of IJobInfoIO that delegates its calls to an IAnyValueManager
 * implementation. This implementation will be used at the server side.
 */
class ServerJobInfoIO : public sup::sequencer::IJobInfoIO
{
public:
  ServerJobInfoIO(const std::string& job_prefix, sup::dto::uint32 n_vars,
                  IAnyValueManager& av_manager);
  virtual ~ServerJobInfoIO();

  void InitNumberOfInstructions(sup::dto::uint32 n_instr) override;

  void InstructionStateUpdated(sup::dto::uint32 instr_idx,
                               sup::sequencer::InstructionState state) override;

  void VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                       bool connected) override;

  void JobStateUpdated(sup::sequencer::JobState state) override;

  void PutValue(const sup::dto::AnyValue& value, const std::string& description) override;

  bool GetUserValue(sup::dto::AnyValue& value, const std::string& description) override;

  int GetUserChoice(const std::vector<std::string>& options,
                    const sup::dto::AnyValue& metadata) override;

  void Message(const std::string& message) override;

  void Log(int severity, const std::string& message) override;

private:
  const std::string m_job_prefix;
  const sup::dto::uint32 m_n_vars;
  IAnyValueManager& m_av_manager;
  IndexGenerator m_log_idx_gen;
  IndexGenerator m_msg_idx_gen;
  IndexGenerator m_out_val_idx_gen;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SERVER_JOB_INFO_IO_H_
