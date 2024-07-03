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

#ifndef SUP_AUTO_SERVER_I_JOB_MANAGER_H_
#define SUP_AUTO_SERVER_I_JOB_MANAGER_H_

#include <sup/auto-server/job_info.h>

#include <sup/sequencer/job_commands.h>

#include <string>
#include <utility>

namespace sup
{
namespace auto_server
{

/**
 * @brief IJobManager defines the API for implementations that manage multiple jobs. The API
 * allows consumers to query static information about the jobs: number of jobs, instruction tree
 * structures, number of variables, etc.
 */
class IJobManager
{
public:
  virtual ~IJobManager();

  /**
   * @brief Get the server prefix. This prefix, together with a knowledge of the used (network)
   * protocol, allows to subscribe to the dynamic information of all supported jobs: instruction
   * statuses, variable values, etc.
   *
   * @return Server prefix.
   */
  virtual std::string GetServerPrefix() const = 0;

  /**
   * @brief Get the number of jobs managed by this object.
   *
   * @return Number of jobs.
   */
  virtual std::size_t GetNumberOfJobs() const = 0;

  /**
   * @brief Get the JobInfo for the specified job.
   *
   * @param job_idx Index that identifies a single job.
   * @return Reference to JobInfo object for the requested job.
   */
  virtual const JobInfo& GetJobInfo(std::size_t job_idx) const = 0;

  /**
   * @brief (De)activate a breakpoint for the given instruction of the specified job.
   *
   * @param job_idx Index that identifies a single job.
   * @param instr_idx Index that identifies a single instruction in the job.
   * @param breakpoint_active True of breakpoint needs to be set, false in the opposite case.
   */
  virtual void EditBreakpoint(std::size_t job_idx, std::size_t instr_idx,
                              bool breakpoint_active) = 0;

  /**
   * @brief Send a JobCommand to the specified job.
   *
   * @param job_idx Index that identifies a single job.
   * @param command JobCommand to send.
   */
  virtual void SendJobCommand(std::size_t job_idx, sup::sequencer::JobCommand command) = 0;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_JOB_MANAGER_H_
