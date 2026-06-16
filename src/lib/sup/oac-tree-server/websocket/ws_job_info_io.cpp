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

#include "ws_job_info_io.h"

#include "periodic_resender.h"

#include <sup/oac-tree-server/websocket/ws_message.h>

#include <sup/oac-tree/user_input_reply.h>

#include <sup/dto/anyvalue_helper.h>

#include <utility>

namespace sup
{
namespace oac_tree_server
{
using sup::oac_tree::InstructionState;

WSJobInfoIO::WSJobInfoIO(Publisher publisher, std::shared_ptr<InputRequestServer> request_server,
                         std::chrono::milliseconds resend_interval)
  : m_publisher{std::move(publisher)}
  , m_request_server{std::move(request_server)}
  , m_resend_interval{resend_interval}
{}

WSJobInfoIO::~WSJobInfoIO() = default;

void WSJobInfoIO::InitNumberOfInstructions(sup::dto::uint32 n_instr)
{
  (void)n_instr;
}

void WSJobInfoIO::InstructionStateUpdated(sup::dto::uint32 instr_idx, InstructionState state)
{
  m_publisher(CreateInstructionStateUpdateMessage(instr_idx, state));
}

void WSJobInfoIO::BreakpointInstructionUpdated(sup::dto::uint32 instr_idx)
{
  m_publisher(CreateBreakpointInstructionUpdateMessage(instr_idx));
}

void WSJobInfoIO::VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                                  bool connected)
{
  m_publisher(CreateVariableStateUpdateMessage(var_idx, connected, value));
}

void WSJobInfoIO::JobStateUpdated(sup::oac_tree::JobState state)
{
  m_publisher(CreateJobStateUpdateMessage(state));
}

void WSJobInfoIO::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{
  m_publisher(CreatePutValueUpdateMessage(description, value));
}

bool WSJobInfoIO::GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                               const std::string& description)
{
  m_request_server->InitNewRequest(id);
  auto message = CreateGetUserValueUpdateMessage(id, description, value);
  m_publisher(message);
  auto [retrieved, reply] = WaitForReplyWithResend(id, message);
  if (!retrieved)
  {
    return false;
  }
  auto [parsed, user_value] = sup::oac_tree::ParseUserValueReply(reply);
  if (!parsed)
  {
    return false;
  }
  // Use TryAssign, since it is up to concrete Instructions to handle possible type mismatches:
  return sup::dto::TryAssign(value, user_value);
}

int WSJobInfoIO::GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                               const sup::dto::AnyValue& metadata)
{
  m_request_server->InitNewRequest(id);
  auto message = CreateGetUserChoiceUpdateMessage(id, options, metadata);
  m_publisher(message);
  auto [retrieved, reply] = WaitForReplyWithResend(id, message);
  if (!retrieved)
  {
    return -1;
  }
  auto [parsed, choice] = sup::oac_tree::ParseUserChoiceReply(reply);
  if (!parsed)
  {
    return -1;
  }
  return choice;
}

void WSJobInfoIO::Interrupt(sup::dto::uint64 id)
{
  m_request_server->Interrupt(id);
}

void WSJobInfoIO::Message(const std::string& message)
{
  m_publisher(CreateTextMessageUpdateMessage(message));
}

void WSJobInfoIO::Log(int severity, const std::string& message)
{
  m_publisher(CreateLogUpdateMessage(severity, message));
}

// Procedure ticks are not forwarded over the network!
void WSJobInfoIO::ProcedureTicked()
{}

std::pair<bool, UserInputReply> WSJobInfoIO::WaitForReplyWithResend(sup::dto::uint64 id,
                                                                   const std::string& message)
{
  // Re-publish the request periodically so that clients connecting after the initial broadcast
  // still receive it. The resender is stopped (and its thread joined) as soon as we return.
  PeriodicResender resender{[this, message]() { m_publisher(message); }, m_resend_interval};
  return m_request_server->WaitForReply(id);
}

}  // namespace oac_tree_server

}  // namespace sup
