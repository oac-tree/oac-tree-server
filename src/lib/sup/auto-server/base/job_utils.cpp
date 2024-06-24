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

#include "job_utils.h"

#include "anyvalue_utils.h"
#include "variable_utils.h"
#include "instruction_tree_utils.h"

#include <sup/auto-server/exceptions.h>
#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{
namespace utils
{

JobInfo ToJobInfo(const sup::dto::AnyValue& job_info_anyvalue)
{
  if (!ValidateJobInfoAnyValue(job_info_anyvalue))
  {
    const std::string error = "ToJobInfo(): wrong format of job info AnyValue";
    throw InvalidOperationException(error);
  }
  auto prefix = job_info_anyvalue[kJobPrefixFieldName].As<std::string>();
  auto full_name = job_info_anyvalue[kFullNameFieldName].As<std::string>();
  auto ws_info = ToWorkspaceInfo(job_info_anyvalue[kWorkspaceInfoFieldName]);
  auto instr_info_tree = ToInstructionInfoTree(job_info_anyvalue[kInstructionTreeInfoFieldName]);
  return JobInfo{ prefix, full_name, ws_info, std::move(instr_info_tree) };
}

sup::dto::AnyValue ToAnyValue(const JobInfo& job_info)
{
  auto result = kJobInfoAnyValue;
  result[kJobPrefixFieldName] = job_info.GetPrefix();
  result[kFullNameFieldName] = job_info.GetProcedureName();
  result[kWorkspaceInfoFieldName] = ToAnyValue(job_info.GetWorkspaceInfo());
  result[kInstructionTreeInfoFieldName] = ToAnyValueTree(*job_info.GetRootInstructionInfo());
  return result;
}

bool ValidateJobInfoAnyValue(const sup::dto::AnyValue& job_info)
{
  if (!ValidateMemberType(job_info, kJobPrefixFieldName, sup::dto::StringType))
  {
    return false;
  }
  if (!ValidateMemberType(job_info, kFullNameFieldName, sup::dto::StringType))
  {
    return false;
  }
  if (!job_info.HasField(kWorkspaceInfoFieldName))
  {
    return false;
  }
  if (!job_info.HasField(kInstructionTreeInfoFieldName))
  {
    return false;
  }
  return true;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
