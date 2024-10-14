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

#include <sup/auto-server/listener_initializer.h>

#include <sup/auto-server/sup_auto_protocol.h>

namespace sup
{
namespace auto_server
{

ListenerInitializer::ListenerInitializer(const std::string& job_prefix, sup::dto::uint32 n_vars,
                                         std::unique_ptr<IAnyValueListener> av_listener)
  : m_job_prefix{job_prefix}
  , m_av_listener{std::move(av_listener)}
{
  auto value_set = GetInitialValueSet(m_job_prefix, n_vars);
  m_av_listener->AddAnyValues(value_set);
  auto input_server_name = GetInputServerName(m_job_prefix);
  m_av_listener->AddInputClient(input_server_name);
}

ListenerInitializer::~ListenerInitializer() = default;

void ListenerInitializer::InitNumberOfInstructions(sup::dto::uint32 n_instr)
{
  auto instr_value_set = GetInstructionValueSet(m_job_prefix, n_instr);
  m_av_listener->AddAnyValues(instr_value_set);
}

}  // namespace auto_server

}  // namespace sup
