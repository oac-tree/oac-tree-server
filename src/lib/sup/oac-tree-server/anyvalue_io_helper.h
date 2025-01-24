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

#ifndef SUP_OAC_TREE_SERVER_ANYVALUE_IO_HELPER_H_
#define SUP_OAC_TREE_SERVER_ANYVALUE_IO_HELPER_H_

#include <sup/oac-tree-server/i_anyvalue_io.h>

namespace sup
{
namespace auto_server
{

void InitializeJobAndVariables(IAnyValueIO& anyvalue_io, const std::string& job_prefix,
                               sup::dto::uint32 n_vars);

void InitializeInstructions(IAnyValueIO& anyvalue_io, const std::string& job_prefix,
                            sup::dto::uint32 n_instr);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_ANYVALUE_IO_HELPER_H_
