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

#ifndef SUP_AUTO_SERVER_EPICS_JOB_PV_HANDLER_H_
#define SUP_AUTO_SERVER_EPICS_JOB_PV_HANDLER_H_

#include "pv_update_queue.h"

#include <future>

namespace sup
{

namespace auto_server
{

/**
 * @brief EPICSJobPVHandler is responsible for serving the EPICS PVs associated with the job state
 * and workspace variables. The current implementation tries to avoid blocking too long in
 * update calls by pushing those on a queue that is processed in a separate thread.
 */
class EPICSJobPVHandler
{
public:
  EPICSJobPVHandler(const std::string& prefix, sup::dto::uint32 n_variables);
  ~EPICSJobPVHandler();

  void UpdateJobState(const sup::dto::AnyValue& job_state);

  void UpdateVariable(sup::dto::uint32 index, const sup::dto::AnyValue& var_info);

private:
  void UpdateLoop(sup::dto::uint32 n_variables);
  PVUpdateQueue m_update_queue;
  const std::string m_prefix;
  std::future<void> m_update_future;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_JOB_PV_HANDLER_H_
