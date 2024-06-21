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

namespace sup
{
namespace auto_server
{
namespace utils
{

JobInfo ToJobInfo(const sup::dto::AnyValue& job_info_anyvalue);

sup::dto::AnyValue ToAnyValue(const JobInfo& job_info);

bool ValidateWorkspaceInfoAnyValue(const sup::dto::AnyValue& job_info);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
