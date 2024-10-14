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

#ifndef SUP_AUTO_SERVER_ANYVALUE_IO_INITIALIZER_H_
#define SUP_AUTO_SERVER_ANYVALUE_IO_INITIALIZER_H_

#include <sup/auto-server/i_anyvalue_io.h>

#include <memory>

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

#endif  // SUP_AUTO_SERVER_ANYVALUE_IO_INITIALIZER_H_
