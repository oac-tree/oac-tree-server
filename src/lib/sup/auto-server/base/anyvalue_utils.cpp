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

#include "anyvalue_utils.h"

namespace sup
{
namespace auto_server
{
namespace utils
{

bool ValidateMemberType(const sup::dto::AnyValue& anyvalue, const std::string& mem_name,
                        const sup::dto::AnyType& mem_type)
{
  if (!anyvalue.HasField(mem_name))
  {
    return false;
  }
  if (anyvalue[mem_name].GetType() != mem_type)
  {
    return false;
  }
  return true;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
