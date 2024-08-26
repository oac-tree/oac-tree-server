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

#ifndef SUP_AUTO_SERVER_ANYVALUE_INPUT_REQUEST_H_
#define SUP_AUTO_SERVER_ANYVALUE_INPUT_REQUEST_H_

#include <sup/dto/anyvalue.h>

#include <string>
#include <vector>

namespace sup
{
namespace auto_server
{

const std::string kInputRequestOptionsFieldName = "options";
const std::string kInputRequestMetadataFieldName = "metadata";

/**
 * @brief Enumeration for the different types of user input.
 *
 */
enum class InputRequestType
{
  kUserValue = 0,
  kUserChoice
};

/**
 * @brief e.
 */
struct AnyValueInputRequest
{
public:
  InputRequestType m_request_type;
  sup::dto::AnyValue m_meta_data;
  sup::dto::AnyType m_input_type;
};

AnyValueInputRequest CreateUserValueRequest(const sup::dto::AnyValue& value,
                                            const std::string& description);

AnyValueInputRequest CreateUserChoiceRequest(const std::vector<std::string>& options,
                                             const sup::dto::AnyValue& metadata);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_ANYVALUE_INPUT_REQUEST_H_
