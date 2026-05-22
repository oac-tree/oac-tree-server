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

#ifndef SUP_OAC_TREE_SERVER_BASE_WS_MESSAGE_JSON_HELPERS_H_
#define SUP_OAC_TREE_SERVER_BASE_WS_MESSAGE_JSON_HELPERS_H_

#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>

#include <rapidjson/document.h>

#include <string>

namespace sup
{
namespace oac_tree_server
{
namespace ws_message_json_helpers
{

std::string SerializeDocumentToJSON(const rapidjson::Document& doc);

}  // namespace ws_message_json_helpers

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_BASE_WS_MESSAGE_JSON_HELPERS_H_
