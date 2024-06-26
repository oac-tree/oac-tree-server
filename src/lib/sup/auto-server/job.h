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

#ifndef SUP_AUTO_SERVER_JOB_H_
#define SUP_AUTO_SERVER_JOB_H_

#include <sup/auto-server/job_info.h>
#include <sup/auto-server/i_anyvalue_manager.h>

#include <sup/sequencer/async_runner.h>
#include <sup/sequencer/instruction.h>
#include <sup/sequencer/procedure.h>

#include <memory>
#include <string>

namespace sup
{
namespace auto_server
{
/**
 * @brief The Job class instantiates a AsyncRunner and an appropriate JobInterface to use.
 * It manages the lifetime of Procedure, AsyncRunner and JobInterface.
 */
class Job
{
public:
  Job(const std::string& prefix, std::unique_ptr<sup::sequencer::Procedure> proc,
      IAnyValueManager& anyvalue_mgr);
  ~Job();

  // Delete copy
  Job(const Job& other) = delete;
  Job& operator=(const Job& other) = delete;

  // Only support move construction
  Job(Job&& other);
  Job& operator=(Job&& other) = delete;

  const JobInfo& GetInfo() const;

  /**
   * @brief Methods forwarded to AsyncRunner
   */
  void SetBreakpoint(std::size_t instr_idx);
  void RemoveBreakpoint(std::size_t instr_idx);
  void Start();
  void Step();
  void Pause();
  void Reset();
  void Halt();

private:
  sup::sequencer::AsyncRunner& Runner();
  struct JobImpl;
  std::unique_ptr<JobImpl> m_impl;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_H_
