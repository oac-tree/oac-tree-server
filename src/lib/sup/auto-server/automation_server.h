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

#include <sup/sequencer/local_job.h>

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
 * an IAnyValueManager implementation, managed by a registry. The JobInterface object will
 * be responsible for mapping Instruction pointers and Variable names to the correct value names and
 * methods of IAnyValueManager.
 */
class AutomationServer : public IJobManager
{
public:
  AutomationServer(const std::string& server_prefix, IAnyValueManagerRegistry& av_mgr_registry);
  virtual ~AutomationServer();

  void AddJob(std::unique_ptr<sup::sequencer::Procedure> proc);

  std::string GetServerPrefix() const override;
  std::size_t GetNumberOfJobs() const override;

  sup::sequencer::JobInfo GetJobInfo(std::size_t job_idx) const override;

  void EditBreakpoint(std::size_t job_idx, std::size_t instr_idx, bool breakpoint_active) override;

  void SendJobCommand(std::size_t job_idx, sup::sequencer::JobCommand command) override;

private:
  sup::sequencer::LocalJob& GetJob(std::size_t job_idx);
  const sup::sequencer::LocalJob& GetJob(std::size_t job_idx) const;
  const std::string m_server_prefix;
  IAnyValueManagerRegistry& m_av_mgr_registry;
  std::vector<std::unique_ptr<sup::sequencer::IJobInfoIO>> m_job_info_ios;
  std::vector<sup::sequencer::LocalJob> m_jobs;
  mutable std::mutex m_mtx;
};

sup::dto::uint32 GetNumberOfVariables(const sup::sequencer::Procedure& proc);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_AUTOMATION_SERVER_H_
