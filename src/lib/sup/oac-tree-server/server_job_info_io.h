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

#ifndef SUP_OAC_TREE_SERVER_SERVER_JOB_INFO_IO_H_
#define SUP_OAC_TREE_SERVER_SERVER_JOB_INFO_IO_H_

#include <sup/oac-tree-server/i_anyvalue_manager.h>
#include <sup/oac-tree-server/index_generator.h>

#include <sup/oac-tree/i_job_info_io.h>

namespace sup
{
namespace oac_tree_server
{
/**
 * @brief Implementation of IJobInfoIO that delegates its calls to an IAnyValueManager
 * implementation. This implementation will be used at the server side.
 */
class ServerJobInfoIO : public sup::oac_tree::IJobInfoIO
{
public:
  ServerJobInfoIO(const std::string& job_prefix, sup::dto::uint32 n_vars,
                  IAnyValueManager& av_manager);
  virtual ~ServerJobInfoIO();

  void InitNumberOfInstructions(sup::dto::uint32 n_instr) override;

  void InstructionStateUpdated(sup::dto::uint32 instr_idx,
                               sup::oac_tree::InstructionState state) override;

  void VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                       bool connected) override;

  void JobStateUpdated(sup::oac_tree::JobState state) override;

  void PutValue(const sup::dto::AnyValue& value, const std::string& description) override;

  bool GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                    const std::string& description) override;

  int GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                    const sup::dto::AnyValue& metadata) override;

  void Interrupt(sup::dto::uint64 id) override;

  void Message(const std::string& message) override;

  void Log(int severity, const std::string& message) override;

  void NextInstructionsUpdated(const std::vector<sup::dto::uint32>& instr_indices) override;

private:
  const std::string m_job_prefix;
  const sup::dto::uint32 m_n_vars;
  IAnyValueManager& m_av_manager;
  IndexGenerator m_log_idx_gen;
  IndexGenerator m_msg_idx_gen;
  IndexGenerator m_out_val_idx_gen;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_SERVER_JOB_INFO_IO_H_
