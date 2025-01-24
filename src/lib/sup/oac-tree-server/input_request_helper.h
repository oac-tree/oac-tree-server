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

#ifndef SUP_OAC_TREE_SERVER_INPUT_REQUEST_HELPER_H_
#define SUP_OAC_TREE_SERVER_INPUT_REQUEST_HELPER_H_

#include <sup/oac-tree/user_input_request.h>

#include <sup/dto/anyvalue.h>

#include <tuple>

namespace sup
{
namespace auto_server
{

using sup::oac_tree::UserInputRequest;

/**
 * @brief Pack an UserInputRequest with the given request id into a base64 encoded AnyValue.
 *
 * @param id Request id.
 * @param input_request UserInputRequest object to encode.
 *
 * @return Encoded AnyValue.
 */
sup::dto::AnyValue EncodeInputRequest(sup::dto::uint64 id, const UserInputRequest& input_request);

/**
 * @brief Decode the packed and encoded AnyValue into an UserInputRequest.
 *
 * @param encoded Encoded AnyValue.
 *
 * @return Tuple of connected state - request id - UserInputRequest.
 */
std::tuple<bool, sup::dto::uint64, UserInputRequest> DecodeInputRequest(
  const sup::dto::AnyValue& encoded);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_INPUT_REQUEST_HELPER_H_
