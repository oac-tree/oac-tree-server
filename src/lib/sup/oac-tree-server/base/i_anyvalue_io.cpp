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

#include <sup/oac-tree-server/i_anyvalue_io.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/oac-tree/constants.h>

#include <algorithm>

namespace sup
{
namespace oac_tree_server
{

IAnyValueIO::~IAnyValueIO() = default;

std::set<std::string> GetNames(const IAnyValueIO::NameAnyValueSet& name_value_set)
{
  std::set<std::string> result;
  auto func = [](const IAnyValueIO::NameAnyValuePair& name_value_pair) {
    return name_value_pair.first;
  };
  std::transform(name_value_set.begin(), name_value_set.end(),
                 std::inserter(result, result.end()), func);
  return result;
}

IAnyValueIO::NameAnyValueSet GetInitialValueSet(const std::string& job_prefix,
                                                sup::dto::uint32 n_vars)
{
  IAnyValueIO::NameAnyValueSet result;
  auto job_value_name = GetJobStatePVName(job_prefix);
  auto job_value = GetJobStateValue(oac_tree::JobState::kInitial);
  result.emplace_back(job_value_name, job_value);
  auto log_entry_name = GetLogEntryName(job_prefix);
  result.emplace_back(log_entry_name, kLogEntryAnyValue);
  auto msg_entry_name = GetMessageEntryName(job_prefix);
  result.emplace_back(msg_entry_name, kMessageEntryAnyValue);
  auto out_val_entry_name = GetOutputValueEntryName(job_prefix);
  result.emplace_back(out_val_entry_name, kOutputValueEntryAnyValue);
  auto next_instr_name = GetNextInstructionsName(job_prefix);
  result.emplace_back(next_instr_name, kNextInstructionsAnyValue);
  for (sup::dto::uint32 var_idx = 0; var_idx < n_vars; ++var_idx)
  {
    const auto name = GetVariablePVName(job_prefix, var_idx);
    result.emplace_back(name, kVariableAnyValue);
  }
  return result;
}

IAnyValueIO::NameAnyValueSet GetInstructionValueSet(const std::string& job_prefix,
                                                    sup::dto::uint32 n_instr)
{
  IAnyValueIO::NameAnyValueSet result;
  for (sup::dto::uint32 instr_idx = 0; instr_idx < n_instr; ++instr_idx)
  {
    const auto name = GetInstructionPVName(job_prefix, instr_idx);
    result.emplace_back(name, sup::oac_tree::Constants::kInstructionStateAnyValue);
  }
  return result;
}

}  // namespace oac_tree_server

}  // namespace sup
