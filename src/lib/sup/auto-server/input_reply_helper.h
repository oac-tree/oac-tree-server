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

#ifndef SUP_AUTO_SERVER_INPUT_REPLY_HELPER_H_
#define SUP_AUTO_SERVER_INPUT_REPLY_HELPER_H_

#include <sup/sequencer/user_input_reply.h>

#include <sup/dto/anyvalue.h>

#include <tuple>

namespace sup
{
namespace auto_server
{

using sup::sequencer::UserInputReply;

/**
 * @brief Pack an UserInputReply with the given request id into a base64 encoded AnyValue.
 *
 * @param id Request id.
 * @param input_reply UserInputReply object to encode.
 *
 * @return Encoded AnyValue.
 */
sup::dto::AnyValue EncodeInputReply(sup::dto::uint64 id, const UserInputReply& input_reply);

/**
 * @brief Decode the packed and encoded AnyValue into an UserInputReply.
 *
 * @param encoded Encoded AnyValue.
 *
 * @return Tuple of connected state - request id - UserInputReply.
 */
std::tuple<bool, sup::dto::uint64, UserInputReply> DecodeInputReply(const dto::AnyValue& encoded);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_INPUT_REPLY_HELPER_H_
