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

#ifndef SUP_AUTO_SERVER_EPICS_INSTRUCTION_PV_HANDLER_H_
#define SUP_AUTO_SERVER_EPICS_INSTRUCTION_PV_HANDLER_H_

#include "anyvalue_update_queue.h"

#include <future>

namespace sup
{

namespace auto_server
{

/**
 * @brief EPICSInstructionPVHandler is responsible for serving the EPICS PV associated with the
 * instruction tree of a single job. The current implementation tries to avoid blocking too long in
 * update calls by pushing those on a queue that is processed in a separate thread.
 */
class EPICSInstructionPVHandler
{
public:
  EPICSInstructionPVHandler(const std::string& prefix, const sup::dto::AnyValue& instr_tree);
  ~EPICSInstructionPVHandler();

  void UpdateInstructionTree(const sup::dto::AnyValue& instr_tree);

private:
  void UpdateLoop(const sup::dto::AnyValue& instr_tree);

  AnyValueUpdateQueue m_update_queue;
  const std::string m_prefix;
  std::future<void> m_update_future;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_EPICS_INSTRUCTION_PV_HANDLER_H_
