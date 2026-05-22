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

#ifndef SUP_OAC_TREE_SERVER_JSON_MESSAGE_HANDLER_H_
#define SUP_OAC_TREE_SERVER_JSON_MESSAGE_HANDLER_H_

#include <sup/oac-tree/i_job_info_io.h>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief Parses JSON update messages and triggers the corresponding methods in IJobInfoIO.
 */
void HandleUpdateMessage(const std::string& msg, sup::oac_tree::IJobInfoIO& job_info_io);

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_JSON_MESSAGE_HANDLER_H_
