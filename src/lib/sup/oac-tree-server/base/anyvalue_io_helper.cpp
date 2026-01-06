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

#include <sup/oac-tree-server/anyvalue_io_helper.h>

#include <sup/oac-tree-server/oac_tree_protocol.h>

namespace sup
{
namespace oac_tree_server
{

void InitializeJobAndVariables(IAnyValueIO& anyvalue_io, const std::string& job_prefix,
                               sup::dto::uint32 n_vars)
{
  auto value_set = GetInitialValueSet(job_prefix, n_vars);
  (void)anyvalue_io.AddAnyValues(value_set);
  auto input_server_name = GetInputServerName(job_prefix);
  (void)anyvalue_io.AddInputHandler(input_server_name);
}

void InitializeInstructions(IAnyValueIO& anyvalue_io, const std::string& job_prefix,
                            sup::dto::uint32 n_instr)
{
  auto instr_value_set = GetInstructionValueSet(job_prefix, n_instr);
  (void)anyvalue_io.AddAnyValues(instr_value_set);
}

}  // namespace oac_tree_server

}  // namespace sup
