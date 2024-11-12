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

#ifndef SUP_AUTO_SERVER_INPUT_REQUEST_HELPER_H_
#define SUP_AUTO_SERVER_INPUT_REQUEST_HELPER_H_

#include <sup/sequencer/user_input_request.h>

#include <sup/dto/anyvalue.h>

#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace sup
{
namespace auto_server
{

/**
 * @brief Pack an UserInputRequest with the given request id into a base64 encoded AnyValue.
 *
 * @param req_id Request id.
 * @param input_request UserInputRequest object to encode.
 *
 * @return Encoded AnyValue.
 */
sup::dto::AnyValue EncodeInputRequest(sup::dto::uint64 req_id,
                                      const sup::sequencer::UserInputRequest& input_request);

/**
 * @brief Decode the packed and encoded AnyValue into an UserInputRequest.
 *
 * @param encoded Encoded AnyValue.
 *
 * @return Tuple of connected state - request id - UserInputRequest.
 */
std::tuple<bool, sup::dto::uint64, sup::sequencer::UserInputRequest>
DecodeInputRequest(const dto::AnyValue& encoded);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INPUT_REQUEST_HELPER_H_
