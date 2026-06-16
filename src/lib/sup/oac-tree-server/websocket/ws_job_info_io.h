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
 * Copyright (c) : 2010-2026 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_OAC_TREE_SERVER_WS_JOB_INFO_IO_H_
#define SUP_OAC_TREE_SERVER_WS_JOB_INFO_IO_H_

#include <sup/oac-tree-server/base/input_request_server.h>

#include <sup/oac-tree/i_job_info_io.h>

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace sup
{
namespace oac_tree_server
{
/**
 * @brief Implementation of IJobInfoIO that delegates its calls to a websocket endpoint.
 */
class WSJobInfoIO : public sup::oac_tree::IJobInfoIO
{
public:
  using Publisher = std::function<void(std::string message)>;
  WSJobInfoIO(Publisher publisher, std::shared_ptr<InputRequestServer> request_server,
              std::chrono::milliseconds resend_interval);
  virtual ~WSJobInfoIO();

  void InitNumberOfInstructions(sup::dto::uint32 n_instr) override;

  void InstructionStateUpdated(sup::dto::uint32 instr_idx,
                               sup::oac_tree::InstructionState state) override;

  void BreakpointInstructionUpdated(sup::dto::uint32 instr_idx) override;

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

  void ProcedureTicked() override;

private:
  /**
   * @brief Block until a reply for the given request id arrives or the request is interrupted,
   * periodically re-publishing the request message so that late-joining clients still receive it.
   */
  std::pair<bool, UserInputReply> WaitForReplyWithResend(sup::dto::uint64 id,
                                                         const std::string& message);

  Publisher m_publisher;
  std::shared_ptr<InputRequestServer> m_request_server;
  std::chrono::milliseconds m_resend_interval;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_WS_JOB_INFO_IO_H_
