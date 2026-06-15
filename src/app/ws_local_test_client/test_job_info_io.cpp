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

#include "test_job_info_io.h"

#include <sup/oac-tree/execution_status.h>
#include <sup/oac-tree/job_states.h>

#include <sup/dto/anyvalue_helper.h>

#include <iostream>

namespace sup
{
namespace oac_tree_server
{
namespace utils
{

TestJobInfoIO::TestJobInfoIO() = default;

TestJobInfoIO::~TestJobInfoIO() = default;

void TestJobInfoIO::InitNumberOfInstructions(sup::dto::uint32 n_instr)
{
  std::cout << "InitNumberOfInstructions: n_instr=" << n_instr << std::endl;
}

void TestJobInfoIO::InstructionStateUpdated(sup::dto::uint32 instr_idx,
                                            sup::oac_tree::InstructionState state)
{
  std::cout << "InstructionStateUpdated: instr_idx=" << instr_idx
            << ", status=" << sup::oac_tree::StatusToString(state.m_execution_status)
            << ", breakpoint=" << std::boolalpha << state.m_breakpoint_set << std::endl;
}

void TestJobInfoIO::BreakpointInstructionUpdated(sup::dto::uint32 instr_idx)
{
  std::cout << "BreakpointInstructionUpdated: instr_idx=" << instr_idx << std::endl;
}

void TestJobInfoIO::VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                                    bool connected)
{
  std::cout << "VariableUpdated: var_idx=" << var_idx
            << ", connected=" << std::boolalpha << connected
            << ", value=" << sup::dto::ValuesToJSONString(value) << std::endl;
}

void TestJobInfoIO::JobStateUpdated(sup::oac_tree::JobState state)
{
  std::cout << "JobStateUpdated: state=" << sup::oac_tree::ToString(state) << std::endl;
}

void TestJobInfoIO::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{
  std::cout << "PutValue: description=" << description
            << ", value=" << sup::dto::ValuesToJSONString(value) << std::endl;
}

bool TestJobInfoIO::GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                                 const std::string& description)
{
  (void)value;
  std::cout << "GetUserValue: id=" << id << ", description=" << description
            << " (not supported, returning false)" << std::endl;
  return false;
}

int TestJobInfoIO::GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                                 const sup::dto::AnyValue& metadata)
{
  (void)metadata;
  std::cout << "GetUserChoice: id=" << id << ", options=[";
  for (std::size_t i = 0; i < options.size(); ++i)
  {
    std::cout << (i == 0 ? "" : ", ") << options[i];
  }
  std::cout << "] (not supported, returning -1)" << std::endl;
  return -1;
}

void TestJobInfoIO::Interrupt(sup::dto::uint64 id)
{
  std::cout << "Interrupt: id=" << id << std::endl;
}

void TestJobInfoIO::Message(const std::string& message)
{
  std::cout << "Message: " << message << std::endl;
}

void TestJobInfoIO::Log(int severity, const std::string& message)
{
  std::cout << "Log: severity=" << severity << ", message=" << message << std::endl;
}

void TestJobInfoIO::ProcedureTicked()
{
  std::cout << "ProcedureTicked" << std::endl;
}

}  // namespace utils

}  // namespace oac_tree_server

}  // namespace sup
