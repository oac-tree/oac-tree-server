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

#ifndef SUP_AUTO_SERVER_ANYVALUE_UTILS_H_
#define SUP_AUTO_SERVER_ANYVALUE_UTILS_H_

#include <sup/dto/anyvalue.h>

#include <string>

namespace sup
{
namespace auto_server
{
namespace utils
{

/**
 * @brief Validate the presence in a given AnyValue of a structure member with the given name
 * and AnyType.
 *
 * @param anyvalue AnyValue to validate.
 * @param mem_name Expected member name.
 * @param mem_type Expected member AnyType.
 * @return true if such a member is present.
 */
bool ValidateMemberType(const sup::dto::AnyValue& anyvalue, const std::string& mem_name,
                        const sup::dto::AnyType& mem_type);

}  // namespace utils

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_ANYVALUE_UTILS_H_
