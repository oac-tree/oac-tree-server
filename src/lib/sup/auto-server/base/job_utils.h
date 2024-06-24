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

#ifndef SUP_AUTO_SERVER_JOB_UTILS_H_
#define SUP_AUTO_SERVER_JOB_UTILS_H_

#include <sup/auto-server/job_info.h>

#include <sup/dto/anyvalue.h>

namespace sup
{
namespace auto_server
{
namespace utils
{

/**
 * @brief Convert the given AnyValue to a JobInfo object.
 *
 * @param job_info_anyvalue AnyValue representation of a Job.
 * @return JobInfo object.
 * @throw InvalidOperationException when the provided anyvalue has the wrong format.
 */
JobInfo ToJobInfo(const sup::dto::AnyValue& job_info_anyvalue);

/**
 * @brief Convert the given JobInfo object to an AnyValue.
 *
 * @param job_info JobInfo object.
 * @return AnyValue representation of the JobInfo object.
 */
sup::dto::AnyValue ToAnyValue(const JobInfo& job_info);

/**
 * @brief Validate if the given AnyValue has the right format to be parsed as a JobInfo object.
 *
 * @param job_info AnyValue to validate.
 * @return true when the provided AnyValue can be correctly parsed to a JobInfo object.
 */
bool ValidateJobInfoAnyValue(const sup::dto::AnyValue& job_info);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_UTILS_H_
