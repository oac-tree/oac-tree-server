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

#include "epics_input_server.h"

namespace sup
{
namespace auto_server
{
EPICSInputServer::EPICSInputServer(const std::string& server_name)
{
  (void)server_name;
}

EPICSInputServer::~EPICSInputServer() = default;
sup::dto::uint64 EPICSInputServer::InitNewRequest()
{
  return 0;
}

std::pair<bool, sup::dto::AnyValue> EPICSInputServer::WaitForReply(sup::dto::uint64 req_idx,
                                                                   double timeout_sec)
{
  (void)req_idx;
  (void)timeout_sec;
  return { false, {} };
}

}  // namespace auto_server

}  // namespace sup

