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

#ifndef SUP_AUTO_SERVER_UNIT_TEST_HELPER_H_
#define SUP_AUTO_SERVER_UNIT_TEST_HELPER_H_

#include <sup/auto-server/i_anyvalue_manager_registry.h>
#include <sup/auto-server/i_job_manager.h>

#include <sup/sequencer/i_job_info_io.h>
#include <sup/dto/anyvalue.h>

#include <gmock/gmock.h>

#include <condition_variable>
#include <map>
#include <mutex>
#include <string>
#include <vector>

const std::string kShortSequenceBody{
R"RAW(
  <Sequence>
    <Wait timeout="0.1"/>
    <Wait timeout="0.1"/>
  </Sequence>
  <Workspace/>
)RAW"};

const std::string kWorkspaceSequenceBody{
R"RAW(
  <Sequence>
    <Message text="Hello"/>
    <Output fromVar="var1" description="Variable is"/>
    <Copy inputVar="one" outputVar="var1"/>
    <Copy inputVar="one" outputVar="var2"/>
  </Sequence>
  <Workspace>
    <Local name="one" type='{"type":"uint32"}' value='1'/>
    <Local name="var1" type='{"type":"uint32"}' value='0'/>
    <Local name="var2" type='{"type":"uint32"}' value='0'/>
  </Workspace>
)RAW"};

const std::string kLongWaitProcedureBody{
R"RAW(
  <Wait timeout="20"/>
  <Workspace/>
)RAW"};

namespace sup
{
namespace auto_server
{
namespace UnitTestHelper
{

class MockJobInfoIO : public sup::sequencer::IJobInfoIO
{
public:
  MOCK_METHOD(void, InitNumberOfInstructions, (sup::dto::uint32), (override));
  MOCK_METHOD(void, InstructionStateUpdated, (sup::dto::uint32, sup::sequencer::InstructionState), (override));
  MOCK_METHOD(void, VariableUpdated, (sup::dto::uint32, const sup::dto::AnyValue&, bool), (override));
  MOCK_METHOD(void, JobStateUpdated, (sup::sequencer::JobState), (override));
  MOCK_METHOD(void, PutValue, (const sup::dto::AnyValue&, const std::string&), (override));
  MOCK_METHOD(bool, GetUserValue, (sup::dto::uint64, sup::dto::AnyValue&, const std::string&), (override));
  MOCK_METHOD(int, GetUserChoice, (sup::dto::uint64, const std::vector<std::string>&, const sup::dto::AnyValue&), (override));
  MOCK_METHOD(void, Interrupt, (sup::dto::uint64), (override));
  MOCK_METHOD(void, Message, (const std::string&), (override));
  MOCK_METHOD(void, Log, (int, const std::string&), (override));
  MOCK_METHOD(void, NextInstructionsUpdated, (const std::vector<sup::dto::uint32>&), (override));
};

class MockAnyValueIO : public IAnyValueIO
{
public:
  MOCK_METHOD(bool, AddAnyValues, (const NameAnyValueSet&), (override));
  MOCK_METHOD(bool, AddInputHandler, (const std::string&), (override));
};

class MockAnyValueManager : public IAnyValueManager
{
public:
  MOCK_METHOD(bool, AddAnyValues, (const NameAnyValueSet&), (override));
  MOCK_METHOD(bool, AddInputHandler, (const std::string&), (override));
  MOCK_METHOD(bool, UpdateAnyValue, (const std::string&, const sup::dto::AnyValue&), (override));
  MOCK_METHOD(UserInputReply, GetUserInput, (const std::string&, sup::dto::uint64, const UserInputRequest&), (override));
  MOCK_METHOD(void, Interrupt, (const std::string&, sup::dto::uint64), (override));
};

class MockJobManager : public IJobManager
{
public:
  MOCK_METHOD(std::string, GetServerPrefix, (), (const override));
  MOCK_METHOD(sup::dto::uint32, GetNumberOfJobs, (), (const override));
  MOCK_METHOD(sup::sequencer::JobInfo, GetJobInfo, (sup::dto::uint32), (const override));
  MOCK_METHOD(void, EditBreakpoint, (sup::dto::uint32, sup::dto::uint32, bool), (override));
  MOCK_METHOD(void, SendJobCommand, (sup::dto::uint32, sup::sequencer::JobCommand), (override));
};

class TestJobInfoIO : public sup::sequencer::IJobInfoIO
{
public:
  TestJobInfoIO();
  ~TestJobInfoIO() = default;
  void InitNumberOfInstructions(sup::dto::uint32 n_instr) override;
  void InstructionStateUpdated(sup::dto::uint32 instr_idx,
                               sup::sequencer::InstructionState state) override;
  void VariableUpdated(sup::dto::uint32 var_idx, const sup::dto::AnyValue& value,
                               bool connected) override;
  void JobStateUpdated(sup::sequencer::JobState state) override;
  void PutValue(const sup::dto::AnyValue& value, const std::string& description) override;
  bool GetUserValue(sup::dto::uint64 id, sup::dto::AnyValue& value,
                            const std::string& description) override;
  int GetUserChoice(sup::dto::uint64 id, const std::vector<std::string>& options,
                            const sup::dto::AnyValue& metadata) override;
  void Interrupt(sup::dto::uint64 id) override;
  void Message(const std::string& message) override;
  void Log(int severity, const std::string& message) override;
  void NextInstructionsUpdated(const std::vector<sup::dto::uint32>& instr_indices) override;

  bool WaitFor(std::function<bool()> pred, double seconds);
  bool WaitForInstructionState(sup::dto::uint32 instr_idx,
                               sup::sequencer::InstructionState state, double seconds);
  bool WaitForVariableValue(sup::dto::uint32 var_idx,
                               const sup::dto::AnyValue& value, double seconds);
  bool WaitForJobState(sup::sequencer::JobState state, double seconds);

  // Public data members for testing:
  sup::dto::uint32 m_n_instr;
  std::map<sup::dto::uint32, sup::sequencer::InstructionState> m_instr_states;
  std::map<sup::dto::uint32, sup::dto::AnyValue> m_var_values;
  std::map<sup::dto::uint32, sup::dto::AnyValue> m_var_connected;
  sup::sequencer::JobState m_job_state;
  std::mutex m_mtx;
  std::condition_variable m_cv;
};

class TestAnyValueManager : public IAnyValueManager
{
public:
  TestAnyValueManager();
  ~TestAnyValueManager();

  bool AddAnyValues(const NameAnyValueSet& name_value_set) override;
  bool AddInputHandler(const std::string& input_server_name) override;
  bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) override;
  UserInputReply GetUserInput(const std::string& input_server_name, sup::dto::uint64 id,
                              const UserInputRequest& request) override;
  void Interrupt(const std::string& input_server_name, sup::dto::uint64 id) override;

  bool HasAnyValue(const std::string& name) const;

  sup::dto::AnyValue GetAnyValue(const std::string& name) const;

  sup::dto::uint32 GetSize() const;

  bool WaitForValue(const std::string& name, const sup::dto::AnyValue& value, double seconds) const;

  void SetUserInputReply(const UserInputReply& reply);

  bool WaitForInputRequest(const UserInputRequest& request, double seconds) const;

  sup::dto::uint32 GetNbrInputRequests() const;

  friend std::ostream& operator<<(std::ostream& stream, const TestAnyValueManager& server_if);
private:
  bool HasAnyValueImpl(const std::string& name) const;
  sup::dto::AnyValue GetAnyValueImpl(const std::string& name) const;
  std::map<std::string, sup::dto::AnyValue> m_value_map;
  UserInputReply m_user_input_reply;
  std::vector<std::pair<std::string, UserInputRequest>> m_input_requests;
  sup::dto::uint32 m_n_input_requests;
  mutable std::mutex m_mtx;
  mutable std::condition_variable m_cv;
};

class TestAnyValueManagerRegistry : public IAnyValueManagerRegistry
{
public:
  TestAnyValueManagerRegistry();
  ~TestAnyValueManagerRegistry();

  IAnyValueManager& GetAnyValueManager(sup::dto::uint32 idx) override;
private:
  TestAnyValueManager m_av_mgr;
};

/**
 * Creates a string representing a valid XML of sequencer procedure by enclosing user provided body
 * between appropriate header and footer.
 */
std::string CreateProcedureString(const std::string& body);

/**
 * Print the provided AnyValue to std::cout.
 */
void DumpAnyValue(const sup::dto::AnyValue& value);

}  // namespace UnitTestHelper

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_UNIT_TEST_HELPER_H_
