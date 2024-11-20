/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - Sequencer
 *
 * Description   : Sequencer for operational procedures
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2023 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "unit_test_helper.h"

#include <sup/dto/anyvalue_helper.h>
#include <sup/sequencer/sequence_parser.h>

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>

namespace sup
{
namespace auto_server
{
namespace UnitTestHelper
{

TestJobInfoIO::TestJobInfoIO()
  : m_n_instr{0}
  , m_instr_states{}
  , m_var_values{}
  , m_var_connected{}
  , m_job_state{sup::sequencer::JobState::kInitial}
  , m_mtx{}
  , m_cv{}
{}

void TestJobInfoIO::InitNumberOfInstructions(sup::dto::uint32 n_instr)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_n_instr = n_instr;
  }
  m_cv.notify_one();
}

void TestJobInfoIO::InstructionStateUpdated(sup::dto::uint32 instr_idx,
                                            sup::sequencer::InstructionState state)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_instr_states[instr_idx] = state;
  }
  m_cv.notify_one();
}

void TestJobInfoIO::VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                                    bool connected)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_var_connected[var_idx] = connected;
    if (connected)
    {
      m_var_values[var_idx] = value;
    }
  }
  m_cv.notify_one();
}

void TestJobInfoIO::JobStateUpdated(sup::sequencer::JobState state)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_job_state = state;
  }
  m_cv.notify_one();
}

void TestJobInfoIO::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{}

bool TestJobInfoIO::GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                                 const std::string& description)
{
  return false;
}

int TestJobInfoIO::GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                                 const sup::dto::AnyValue& metadata)
{
  return -1;
}

void TestJobInfoIO::Interrupt(sup::dto::uint64 id)
{}

void TestJobInfoIO::Message(const std::string& message)
{}

void TestJobInfoIO::Log(int severity, const std::string& message)
{}

void TestJobInfoIO::NextInstructionsUpdated(const std::vector<sup::dto::uint32>& instr_indices)
{}

bool TestJobInfoIO::WaitFor(std::function<bool()> pred, double seconds)
{
  auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
  std::unique_lock<std::mutex> lk{m_mtx};
  return m_cv.wait_for(lk, duration, pred);
}

bool TestJobInfoIO::WaitForInstructionState(sup::dto::uint32 instr_idx,
                                            sup::sequencer::InstructionState state, double seconds)
{
  auto pred = [this, instr_idx, state](){
    auto instr_state_it = m_instr_states.find(instr_idx);
    if (instr_state_it == m_instr_states.end())
    {
      return false;
    }
    return instr_state_it->second == state;
  };
  return WaitFor(pred, seconds);
}

bool TestJobInfoIO::WaitForVariableValue(sup::dto::uint32 var_idx,
                                         const sup::dto::AnyValue& value, double seconds)
{
  auto pred = [this, var_idx, value](){
    auto var_val_it = m_var_values.find(var_idx);
    if (var_val_it == m_var_values.end())
    {
      return false;
    }
    return var_val_it->second == value;
  };
  return WaitFor(pred, seconds);
}

bool TestJobInfoIO::WaitForJobState(sup::sequencer::JobState state, double seconds)
{
  auto pred = [this, state](){
    return m_job_state == state;
  };
  return WaitFor(pred, seconds);
}

TestAnyValueManager::TestAnyValueManager()
    : m_value_map{}
    , m_user_input_reply{sup::sequencer::kInvalidUserInputReply}
    , m_input_requests{}
    , m_mtx{}
    , m_cv{}
    , m_n_input_requests{}
{}

TestAnyValueManager::~TestAnyValueManager() = default;

bool TestAnyValueManager::AddAnyValues(const NameAnyValueSet& name_value_set)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    for (const auto& name : GetNames(name_value_set))
    {
      if (HasAnyValueImpl(name))
      {
        return false;
      }
    }
    for (const auto& name_value_pair : name_value_set)
    {
      m_value_map[name_value_pair.first] = name_value_pair.second;
    }
  }
  m_cv.notify_one();
  return true;
}

bool TestAnyValueManager::AddInputHandler(const std::string& input_server_name)
{
  (void)input_server_name;
  return true;
}

bool TestAnyValueManager::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    auto iter = m_value_map.find(name);
    if (iter == m_value_map.end())
    {
      return false;
    }
    iter->second = value;
  }
  m_cv.notify_one();
  return true;
}

UserInputReply TestAnyValueManager::GetUserInput(const std::string& input_server_name,
                                                 sup::dto::uint64 id,
                                                 const UserInputRequest& request)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  m_input_requests.emplace_back(input_server_name, request);
  ++m_n_input_requests;
  m_cv.notify_one();
  return m_user_input_reply;
}

void TestAnyValueManager::Interrupt(const std::string& input_server_name, sup::dto::uint64 id)
{
  (void)input_server_name;
  (void)id;
  return;
}

bool TestAnyValueManager::HasAnyValue(const std::string& name) const
{
  std::lock_guard<std::mutex> lk{m_mtx};
  return HasAnyValueImpl(name);
}

sup::dto::AnyValue TestAnyValueManager::GetAnyValue(const std::string& name) const
{
  std::lock_guard<std::mutex> lk{m_mtx};
  return GetAnyValueImpl(name);
}

sup::dto::uint32 TestAnyValueManager::GetSize() const
{
  std::lock_guard<std::mutex> lk{m_mtx};
  return m_value_map.size();
}

bool TestAnyValueManager::WaitForValue(const std::string& name, const sup::dto::AnyValue& value,
                                       double seconds) const
{
  auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
  std::unique_lock<std::mutex> lk{m_mtx};
  auto pred = [this, name, value](){
    return GetAnyValueImpl(name) == value;
  };
  return m_cv.wait_for(lk, duration, pred);
}

void TestAnyValueManager::SetUserInputReply(const UserInputReply& reply)
{
  std::unique_lock<std::mutex> lk{m_mtx};
  m_user_input_reply = reply;
}

bool TestAnyValueManager::WaitForInputRequest(const UserInputRequest& request,
                                              double seconds) const
{
  auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
  std::unique_lock<std::mutex> lk{m_mtx};
  auto pred = [this, request](){
    if (m_input_requests.size() == 0)
    {
      return false;
    }
    return m_input_requests.back().second == request;
  };
  return m_cv.wait_for(lk, duration, pred);
}

sup::dto::uint32 TestAnyValueManager::GetNbrInputRequests() const
{
  std::unique_lock<std::mutex> lk{m_mtx};
  return m_n_input_requests;
}

bool TestAnyValueManager::HasAnyValueImpl(const std::string& name) const
{
  auto iter = m_value_map.find(name);
  return iter != m_value_map.end();
}

sup::dto::AnyValue TestAnyValueManager::GetAnyValueImpl(const std::string& name) const
{
  auto iter = m_value_map.find(name);
  if (iter == m_value_map.end())
  {
    return {};
  }
  return iter->second;
}

std::ostream& operator<<(std::ostream& stream, const TestAnyValueManager& server_if)
{
  for (const auto& name_value_pair : server_if.m_value_map)
  {
    stream << name_value_pair.first << std::endl;
    stream << sup::dto::PrintAnyValue(name_value_pair.second);
    stream << std::endl << std::endl;
  }
  return stream;
}

TestAnyValueManagerRegistry::TestAnyValueManagerRegistry()
  : m_av_mgr{}
{}

TestAnyValueManagerRegistry::~TestAnyValueManagerRegistry() = default;

IAnyValueManager& TestAnyValueManagerRegistry::GetAnyValueManager(sup::dto::uint32 idx)
{
  (void)idx;
  return m_av_mgr;
}

std::string CreateProcedureString(const std::string &body)
{
  static const std::string header{
      R"RAW(<?xml version="1.0" encoding="UTF-8"?>
<Procedure xmlns="http://codac.iter.org/sup/sequencer" version="1.0"
           name="Common header"
           xmlns:xs="http://www.w3.org/2001/XMLSchema-instance"
           xs:schemaLocation="http://codac.iter.org/sup/sequencer sequencer.xsd">)RAW"};

  static const std::string footer{R"RAW(</Procedure>)RAW"};

  return header + body + footer;
}

void DumpAnyValue(const sup::dto::AnyValue& value)
{
  std::cout << sup::dto::PrintAnyValue(value) << std::endl;
}

}  // namespace UnitTestHelper

}  // namespace auto_server

}  // namespace sup
