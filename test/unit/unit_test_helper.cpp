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

TestJobPVServer::TestJobPVServer()
  : m_status_updates{}
  , m_breakpoint_updates{}
  , m_job_state{sequencer::JobState::kInitial}
  , m_mtx{}
  , m_cv{}
{}

TestJobPVServer::~TestJobPVServer() = default;

void TestJobPVServer::UpdateJobStatePV(sequencer::JobState state)
{
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_job_state = state;
  }
  m_cv.notify_one();
}

void TestJobPVServer::UpdateInstructionStatusPV(const sequencer::Instruction* instruction,
                                             sequencer::ExecutionStatus status)
{
  ++m_status_updates[status];
}

void TestJobPVServer::UpdateInstructionBreakpointPV(const sequencer::Instruction*, bool)
{
  ++m_breakpoint_updates;
}

sup::dto::uint32 TestJobPVServer::GetInstructionUpdateCount(sequencer::ExecutionStatus status) const
{
  auto iter = m_status_updates.find(status);
  if (iter == m_status_updates.end())
  {
    return 0;
  }
  return iter->second;
}

sup::dto::uint32 TestJobPVServer::GetBreakpointUpdateCount() const
{
  return m_breakpoint_updates;
}

bool TestJobPVServer::WaitForState(sequencer::JobState state, double seconds) const
{
  auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
  std::unique_lock<std::mutex> lk{m_mtx};
  return m_cv.wait_for(lk, duration, [this, state](){
    return m_job_state == state;
  });
}

sequencer::JobState TestJobPVServer::GetJobState() const
{
  std::lock_guard<std::mutex> lk{m_mtx};
  return m_job_state;
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

sup::dto::AnyValue CreateTestInstructionTreeAnyValue()
{
  const auto procedure_string = CreateProcedureString(kShortSequenceBody);
  auto proc = sequencer::ParseProcedureString(procedure_string);
  proc->Setup();

  auto root_instr = proc->RootInstruction();
  InstructionTreeCache tree_cache{root_instr};
  return tree_cache.GetInitialInstructionTreeAnyValue();
}

}  // namespace UnitTestHelper

}  // namespace auto_server

}  // namespace sup
