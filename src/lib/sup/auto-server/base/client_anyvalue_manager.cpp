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

#include <sup/auto-server/client_anyvalue_manager.h>

#include <sup/auto-server/sup_auto_protocol.h>

namespace
{
using namespace sup::auto_server;
void UpdateJobState(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue);
void UpdateInstructionState(IJobInfoIO& job_info_io, sup::dto::uint32 instr_idx,
                            const sup::dto::AnyValue& anyvalue);
void UpdateVariableSate(IJobInfoIO& job_info_io, sup::dto::uint32 var_idx,
                        const sup::dto::AnyValue& anyvalue);
}  // unnamed namespace

namespace sup
{
namespace auto_server
{

ClientAnyValueManager::ClientAnyValueManager(IJobInfoIO& job_info_io)
  : m_job_info_io{job_info_io}
  , m_cb_map{}
{}

ClientAnyValueManager::~ClientAnyValueManager() = default;

bool ClientAnyValueManager::AddAnyValues(const NameAnyValueSet& name_value_set)
{
  for (auto& name_value_pair : name_value_set)
  {
    auto& value_name = name_value_pair.first;
    if (m_cb_map.find(value_name) != m_cb_map.end())
    {
      return false;
    }
    auto cb = CreateCallback(value_name);
    cb(m_job_info_io, name_value_pair.second);
    m_cb_map[value_name] = cb;
  }
  return true;
}

bool ClientAnyValueManager::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  auto iter = m_cb_map.find(name);
  if (iter == m_cb_map.end())
  {
    return false;
  }
  iter->second(m_job_info_io, value);
  return true;
}

ClientAnyValueManager::AnyValueCallback CreateCallback(const std::string& value_name)
{
  auto ignore = [](IJobInfoIO& job_info_io, const sup::dto::AnyValue& value) {
    (void)job_info_io;
    (void)value;
    return;
  };
  auto info = ParseValueName(value_name);
  auto idx = info.idx;
  switch (info.val_type)
  {
  case ValueNameType::kJobStatus:
    return UpdateJobState;
  case ValueNameType::kInstruction:
    return [idx](IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue) {
      return UpdateInstructionState(job_info_io, idx, anyvalue);
    };
  case ValueNameType::kVariable:
    return [idx](IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue) {
      return UpdateVariableSate(job_info_io, idx, anyvalue);
    };
  case ValueNameType::kUnknown:
    break;
  default:
    break;
  }
  return ignore;
}

}  // namespace auto_server

}  // namespace sup

namespace
{
void UpdateJobState(IJobInfoIO& job_info_io, const sup::dto::AnyValue& anyvalue)
{
  if (!anyvalue.HasField(kJobStateField) ||
      anyvalue[kJobStateField].GetType() != sup::dto::UnsignedInteger32Type)
  {
    return;
  }
  auto job_state_int = anyvalue[kJobStateField].As<sup::dto::uint32>();
  auto job_state = static_cast<sup::sequencer::JobState>(job_state_int);
  job_info_io.OnStateChange(job_state);
}

void UpdateInstructionState(IJobInfoIO& job_info_io, sup::dto::uint32 instr_idx,
                            const sup::dto::AnyValue& anyvalue)
{
  if (anyvalue.HasField(kExecStatusField) &&
      anyvalue[kExecStatusField].GetType() == sup::dto::UnsignedInteger16Type)
  {
    auto exec_state_int = anyvalue[kExecStatusField].As<sup::dto::uint16>();
    auto exec_state = static_cast<sup::sequencer::ExecutionStatus>(exec_state_int);
    job_info_io.UpdateInstructionStatus(instr_idx, exec_state);
  }
  if (anyvalue.HasField(kBreakpointField) &&
      anyvalue[kBreakpointField].GetType() == sup::dto::BooleanType)
  {
    auto breakpoint_set = anyvalue[kBreakpointField].As<sup::dto::boolean>();
    job_info_io.OnBreakpointChange(instr_idx, breakpoint_set);
  }
}

void UpdateVariableSate(IJobInfoIO& job_info_io, sup::dto::uint32 var_idx,
                        const sup::dto::AnyValue& anyvalue)
{
  auto var_state = DecodeVariableState(anyvalue);
  if (sup::dto::IsEmptyValue(var_state.first))
  {
    return;
  }
  job_info_io.VariableUpdated(var_idx, var_state.first, var_state.second);
}

}  // unnamed namespace
