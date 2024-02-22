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

#ifndef SUP_AUTO_SERVER_EPICS_PV_HANDLER_H_
#define SUP_AUTO_SERVER_EPICS_PV_HANDLER_H_

#include "pv_update_queue.h"

#include <sup/epics/pv_access_server.h>

#include <future>

namespace sup
{

namespace auto_server
{

/**
 * @brief EPICSPVHandler is responsible for serving the EPICS PVs associated with the job state and
 * the instruction tree status. The current implementation tries to avoid blocking too long in
 * update calls by pushing those on a queue that is processed in a separate thread.
 */
class EPICSPVHandler
{
public:
  EPICSPVHandler(const std::string& prefix, const sup::dto::AnyValue& instr_tree);
  ~EPICSPVHandler();

  void UpdateJobState(const sup::dto::AnyValue& job_state);

  void UpdateInstructionTree(const sup::dto::AnyValue& instr_tree);

private:
  void UpdateLoop();
  const std::string m_jobstate_channel;
  const std::string m_instruction_tree_channel;
  PVUpdateQueue m_update_queue;
  std::future<void> m_update_future;
  sup::epics::PvAccessServer m_server;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_PV_HANDLER_H_
