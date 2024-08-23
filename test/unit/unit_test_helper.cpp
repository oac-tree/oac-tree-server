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
  , m_instr_statuses{}
  , m_job_states{}
{}

TestJobInfoIO::~TestJobInfoIO() = default;

sup::dto::uint32 TestJobInfoIO::GetNumberOfInstructions() const
{
  return m_n_instr;
}

TestJobInfoIO::InstructionStates TestJobInfoIO::GetInstructionStates() const
{
  return m_instr_statuses;
}

TestJobInfoIO::JobStates TestJobInfoIO::GetJobStates() const
{
  return m_job_states;
}

void TestJobInfoIO::InitNumberOfInstructions(sup::dto::uint32 n_instr)
{
  m_n_instr = n_instr;
}

void TestJobInfoIO::InstructionStateUpdated(sup::dto::uint32 instr_idx, InstructionState state)
{
  m_instr_statuses.emplace_back(instr_idx, state);
}

void TestJobInfoIO::VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                                      bool connected)
{
}

bool TestJobInfoIO::PutValue(const sup::dto::AnyValue& value, const std::string& description)
{
  (void)value;
  (void)description;
  return true;
}

bool TestJobInfoIO::GetUserValue(sup::dto::AnyValue& value, const std::string& description)
{
  (void)value;
  (void)description;
  return false;
}

int TestJobInfoIO::GetUserChoice(const std::vector<std::string>& options,
                    const sup::dto::AnyValue& metadata)
{
  (void)options;
  (void)metadata;
  return -1;
}

void TestJobInfoIO::Message(const std::string& message)
{
  (void)message;
}

void TestJobInfoIO::Log(int severity, const std::string& message)
{
  (void)severity;
  (void)message;
}

void TestJobInfoIO::JobStateUpdated(sup::sequencer::JobState state)
{
  m_job_states.push_back(state);
}

TestAnyValueManager::TestAnyValueManager()
    : m_value_map{}
{}

TestAnyValueManager::~TestAnyValueManager() = default;

bool TestAnyValueManager::AddAnyValues(const NameAnyValueSet& name_value_set)
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
  m_cv.notify_one();
  return true;
}

bool TestAnyValueManager::AddInputServer(const std::string& input_server_name)
{
  (void)input_server_name;
  return true;
}

bool TestAnyValueManager::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  auto iter = m_value_map.find(name);
  if (iter == m_value_map.end())
  {
    return false;
  }
  iter->second = value;
  m_cv.notify_one();
  return true;
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

std::size_t TestAnyValueManager::GetSize() const
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

IAnyValueManager& TestAnyValueManagerRegistry::GetAnyValueManager(std::size_t idx)
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
