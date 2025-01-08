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

#ifndef SUP_AUTO_SERVER_I_CLIENT_JOB_H_
#define SUP_AUTO_SERVER_I_CLIENT_JOB_H_

#include <sup/auto-server/i_anyvalue_io.h>
#include <sup/auto-server/i_job_manager.h>

#include <sup/sequencer/i_job.h>
#include <sup/sequencer/i_job_info_io.h>

#include <memory>

namespace sup
{
namespace auto_server
{
class ClientJobImpl;
/**
 * @brief ClientJob creates and manages the different components required to monitor and interact
 * with a job on the client side. It uses a provided IJobInfoIO object to handle all updates and
 * input/output requests.
 */
class ClientJob : public sup::sequencer::IJob
{
public:
  ClientJob(IJobManager& job_manager, sup::dto::uint32 job_idx,
            const AnyValueIOFactoryFunction& factory_func,
            sup::sequencer::IJobInfoIO& job_info_io);
  ~ClientJob();

  ClientJob(ClientJob&& other);
  ClientJob& operator=(ClientJob&& other);

  const sup::sequencer::JobInfo& GetInfo() const override;
  void SetBreakpoint(sup::dto::uint32 instr_idx) override;
  void RemoveBreakpoint(sup::dto::uint32 instr_idx) override;
  void Start() override;
  void Step() override;
  void Pause() override;
  void Reset() override;
  void Halt() override;

private:
  std::unique_ptr<ClientJobImpl> m_impl;
};

/**
 * @brief Create a client job that will forward all updates and user IO to the passed IJobInfoIO.
 *
 * @param job_manager JobManager object that retrieves JobInfo and is used to control the job.
 * @param job_idx Index of the job.
 * @param factory_func Function that creates an IAnyValueIO object to listen for updates and user
 * IO requests.
 * @param job_info_io User provided IJobInfoIO object that will receive all updates and user IO.
 * @return An IJob object on success. An empty unique_ptr on failure.
 */
std::unique_ptr<sup::sequencer::IJob> CreateClientJob(
    IJobManager &job_manager, sup::dto::uint32 job_idx,
    const AnyValueIOFactoryFunction &factory_func, sup::sequencer::IJobInfoIO &job_info_io);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_CLIENT_JOB_H_
