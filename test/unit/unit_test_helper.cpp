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

#include <fstream>
#include <iostream>

namespace sup
{
namespace auto_server
{
namespace UnitTestHelper
{

CoutPVServer::CoutPVServer()
  : m_instr_update_count{}
{}

CoutPVServer::~CoutPVServer() = default;

void CoutPVServer::UpdateInstructionStatusPV(const sequencer::Instruction* instruction,
                                             sequencer::ExecutionStatus status)
{
  ++m_instr_update_count;
  std::cout << "Instruction status updated:" << std::endl;
  std::cout << "===========================" << std::endl;
  std::cout << static_cast<const void*>(instruction) << std::endl;
  std::cout << sequencer::StatusToString(status) << std::endl;
  std::cout << std::endl;
}

void CoutPVServer::UpdateInstructionBreakpointPV(const sequencer::Instruction* instruction,
                                                 bool breakpoint_set)
{
  ++m_instr_update_count;
  std::cout << "Instruction breakpoint updated:" << std::endl;
  std::cout << "===============================" << std::endl;
  std::cout << static_cast<const void*>(instruction) << std::endl;
  std::cout << std::boolalpha << breakpoint_set << std::endl;
  std::cout << std::endl;
}

sup::dto::uint32 CoutPVServer::GetInstructionUpdateCount() const
{
  return m_instr_update_count;
}

TemporaryTestFile::TemporaryTestFile(std::string filename, std::string contents)
    : m_filename{filename}
{
  std::ofstream file_out(m_filename);
  file_out.write(contents.c_str(), contents.size());
}

TemporaryTestFile::~TemporaryTestFile()
{
  std::remove(m_filename.c_str());
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

}  // namespace UnitTestHelper

}  // namespace auto_server

}  // namespace sup
