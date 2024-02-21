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

#ifndef SUP_AUTO_SERVER_SUP_AUTO_PROTOCOL_H_
#define SUP_AUTO_SERVER_SUP_AUTO_PROTOCOL_H_

#include <string>

namespace sup
{
namespace auto_server
{
// Prefix suggestion: e.g. for CC2D loop:
// "CSW-S2D-AUTO:"
// Prefix for a specific procedure:
// "CSW-S2D-AUTO-STARTUP:

// Instruction tree postfix:
const std::string kInstructionTreeId = "INST";
// Instruction tree node fields:
const std::string kInstructionNodeType = "sup::instructionNode/v1.0";
const std::string kExecStatusField = "exec_status";
const std::string kBreakpointField = "breakpoint";
const std::string kChildrenField = "children";

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SUP_AUTO_PROTOCOL_H_
