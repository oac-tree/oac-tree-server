/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : oac-tree server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "ws_job_info_io.h"

#include <sup/oac-tree-server/anyvalue_io_helper.h>
#include <sup/oac-tree-server/input_request_helper.h>
#include <sup/oac-tree-server/output_entry_helper.h>
#include <sup/oac-tree-server/output_entry_types.h>
#include <sup/oac-tree-server/oac_tree_protocol.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/oac-tree/user_input_reply.h>

namespace sup
{
namespace oac_tree_server
{
using sup::oac_tree::InstructionState;

WSJobInfoIO::WSJobInfoIO(sup::dto::uint32 job_idx, sup::dto::uint32 n_vars)
  : m_job_idx{job_idx}
  , m_n_vars{n_vars}
  , m_log_idx_gen{}
  , m_msg_idx_gen{}
  , m_out_val_idx_gen{}
{}

WSJobInfoIO::~WSJobInfoIO() = default;

void WSJobInfoIO::InitNumberOfInstructions(sup::dto::uint32 n_instr)
{
  (void)n_instr;
}

void WSJobInfoIO::InstructionStateUpdated(sup::dto::uint32 instr_idx, InstructionState state)
{
  (void)instr_idx;
  (void)state;
}

void WSJobInfoIO::BreakpointInstructionUpdated(sup::dto::uint32 instr_idx)
{
  (void)instr_idx;
}

void WSJobInfoIO::VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                                      bool connected)
{
  (void)var_idx;
  (void)value;
  (void)connected;
}

void WSJobInfoIO::JobStateUpdated(sup::oac_tree::JobState state)
{
  (void)state;
}

void WSJobInfoIO::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{
  (void)value;
  (void)description;
}

bool WSJobInfoIO::GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                                   const std::string& description)
{
  (void)id;
  (void)value;
  (void)description;
  return false;
}

int WSJobInfoIO::GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                               const sup::dto::AnyValue& metadata)
{
  (void)id;
  (void)options;
  (void)metadata;
  return -1;
}

void WSJobInfoIO::Interrupt(sup::dto::uint64 id)
{
  (void)id;
}

void WSJobInfoIO::Message(const std::string& message)
{
  (void)message;
}

void WSJobInfoIO::Log(int severity, const std::string& message)
{
  (void)severity;
  (void)message;
}

// Procedure ticks are not forwarded over the network!
void WSJobInfoIO::ProcedureTicked()
{}

}  // namespace oac_tree_server

}  // namespace sup
