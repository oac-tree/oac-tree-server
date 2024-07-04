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

#ifndef SUP_AUTO_SERVER_AUTOMATION_SERVER_H_
#define SUP_AUTO_SERVER_AUTOMATION_SERVER_H_

#include <sup/auto-server/i_anyvalue_manager_registry.h>
#include <sup/auto-server/i_job_manager.h>
#include <sup/auto-server/job.h>

#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace sup
{
namespace auto_server
{
/**
 * @brief Server implementation of the IJobManager interface. This class manages multiple jobs,
 * i.e. procedures and their associated input/output interfaces. For each job, the class will
 * instantiate an appropriate JobInterface implementation that will delegate its method calls to
 * an IAnyValueManager implementation, managed by a registry. The AutomationJobInterface object will
 * be responsible for mapping Instruction pointers and Variable names to plain string identifiers,
 * allowing the IAnyValueManager to use these as channels for network implementations.
 */
class AutomationServer : public IJobManager
{
public:
  AutomationServer(const std::string& server_prefix, IAnyValueManagerRegistry& av_mgr_registry);
  virtual ~AutomationServer();

  void AddJob(std::unique_ptr<sup::sequencer::Procedure> proc);

  std::string GetServerPrefix() const override;
  std::size_t GetNumberOfJobs() const override;

  const JobInfo& GetJobInfo(std::size_t job_idx) const override;

  void EditBreakpoint(std::size_t job_idx, std::size_t instr_idx, bool breakpoint_active) override;

  void SendJobCommand(std::size_t job_idx, sup::sequencer::JobCommand command) override;

private:
  Job& GetJob(std::size_t job_idx);
  const Job& GetJob(std::size_t job_idx) const;
  const std::string m_server_prefix;
  IAnyValueManagerRegistry& m_av_mgr_registry;
  std::vector<Job> m_jobs;
  mutable std::mutex m_mtx;
};

std::string CreateJobPrefix(const std::string& server_prefix, std::size_t idx);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_AUTOMATION_SERVER_H_
