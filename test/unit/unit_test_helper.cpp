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

#include <sup/auto-server/instruction_tree_cache.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/sequencer/sequence_parser.h>

#include <chrono>
#include <cmath>
#include <fstream>

namespace sup
{
namespace auto_server
{
namespace UnitTestHelper
{

TestServerInterface::TestServerInterface()
    : m_value_map{}
{}

TestServerInterface::~TestServerInterface() = default;

bool TestServerInterface::ServeAnyValues(const NameAnyValueSet& name_value_set)
{
  for (const auto& name : GetNames(name_value_set))
  {
    if (HasAnyValue(name))
    {
      return false;
    }
  }
  for (const auto& name_value_pair : name_value_set)
  {
    m_value_map[name_value_pair.first] = name_value_pair.second;
  }
  return true;
}

bool TestServerInterface::UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value)
{
  auto iter = m_value_map.find(name);
  if (iter == m_value_map.end())
  {
    return false;
  }
  iter->second = value;
  return true;
}

bool TestServerInterface::HasAnyValue(const std::string& name) const
{
  auto iter = m_value_map.find(name);
  return iter != m_value_map.end();
}

sup::dto::AnyValue TestServerInterface::GetAnyValue(const std::string& name) const
{
  auto iter = m_value_map.find(name);
  if (iter == m_value_map.end())
  {
    return {};
  }
  return iter->second;
}

std::size_t TestServerInterface::GetSize() const
{
  return m_value_map.size();
}

std::ostream& operator<<(std::ostream& stream, const TestServerInterface& server_if)
{
  for (const auto& name_value_pair : server_if.m_value_map)
  {
    stream << name_value_pair.first << std::endl;
    stream << sup::dto::PrintAnyValue(name_value_pair.second);
    stream << std::endl << std::endl;
  }
  return stream;
}

std::string CreateProcedureString(const std::string &body)
{
  static const std::string header{
      R"RAW(<?xml version="1.0" encoding="UTF-8"?>
<Procedure xmlns="http://codac.iter.org/sup/sequencer" version="1.0"
           name="Trivial procedure for testing purposes"
           xmlns:xs="http://www.w3.org/2001/XMLSchema-instance"
           xs:schemaLocation="http://codac.iter.org/sup/sequencer sequencer.xsd">)RAW"};

  static const std::string footer{R"RAW(</Procedure>)RAW"};

  return header + body + footer;
}

sup::dto::AnyValue CreateTestInstructionTreeAnyValue()
{
  const auto procedure_string = CreateProcedureString(kShortSequenceBody);
  auto proc = sequencer::ParseProcedureString(procedure_string);
  proc->Setup();

  auto root_instr = proc->RootInstruction();
  InstructionTreeCache tree_cache{};
  tree_cache.InitializeCache(root_instr);
  return tree_cache.GetInitialInstructionTreeAnyValue();
}

}  // namespace UnitTestHelper

}  // namespace auto_server

}  // namespace sup
