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

#ifndef SUP_AUTO_SERVER_SERVER_JOB_INTERFACE_H_
#define SUP_AUTO_SERVER_SERVER_JOB_INTERFACE_H_

#include <sup/auto-server/instruction_tree_cache.h>
#include <sup/auto-server/job_value_mapper.h>
#include <sup/auto-server/server_interface.h>

#include <sup/sequencer/job_interface.h>

namespace sup
{
namespace auto_server
{

class ServerJobInterface : public sequencer::JobInterface
{
public:
  ServerJobInterface(const std::string& prefix, const sequencer::Procedure& proc,
                     ServerInterface& server_interface);

  ~ServerJobInterface();

  void InitializeInstructionTree(const sequencer::Instruction* root);

  void UpdateInstructionStatus(const sequencer::Instruction* instruction) override;
  void VariableUpdated(const std::string& name, const sup::dto::AnyValue& value,
                       bool connected) override;
  bool PutValue(const sup::dto::AnyValue& value, const std::string& description) override;
  bool GetUserValue(sup::dto::AnyValue& value, const std::string& description) override;
  int GetUserChoice(const std::vector<std::string>& options,
                    const sup::dto::AnyValue& metadata) override;
  void Message(const std::string& message) override;
  void Log(int severity, const std::string& message) override;

  void OnStateChange(sequencer::JobState state) noexcept override;
  void OnBreakpointChange(const sequencer::Instruction* instruction,
                          bool breakpoint_set) noexcept override;
  void OnProcedureTick(const sequencer::Procedure& proc) noexcept override;

private:
  ServerInterface::NameAnyValueSet GetValueSet() const;
  const JobValueMapper m_job_value_mapper;
  InstructionTreeCache m_instr_tree_cache;
  sup::dto::AnyValue m_instr_tree_anyvalue;
  ServerInterface& m_server_interface;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SERVER_JOB_INTERFACE_H_
