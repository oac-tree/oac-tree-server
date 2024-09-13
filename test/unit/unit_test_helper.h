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
#include <sup/auto-server/i_job_info_io.h>

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
// TODO: use mock methods
class TestJobInfoIO : public IJobInfoIO
{
public:
  MOCK_METHOD1(InitNumberOfInstructions, void(sup::dto::uint32));
  MOCK_METHOD2(InstructionStateUpdated, void(sup::dto::uint32, InstructionState));
  MOCK_METHOD3(VariableUpdated, void(sup::dto::uint32, const sup::dto::AnyValue&, bool));
  MOCK_METHOD1(JobStateUpdated, void(sup::sequencer::JobState));
  MOCK_METHOD2(PutValue, bool(const sup::dto::AnyValue&, const std::string&));
  MOCK_METHOD2(GetUserValue, bool(sup::dto::AnyValue&, const std::string&));
  MOCK_METHOD2(GetUserChoice, int(const std::vector<std::string>&, const sup::dto::AnyValue&));
  MOCK_METHOD1(Message, void(const std::string&));
  MOCK_METHOD2(Log, void(int, const std::string&));
};

class TestAnyValueManager : public IAnyValueManager
{
public:
  TestAnyValueManager();
  ~TestAnyValueManager();

  bool AddAnyValues(const NameAnyValueSet& name_value_set) override;
  bool AddInputServer(const std::string& input_server_name) override;
  bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) override;
  sup::dto::AnyValue GetUserInput(const std::string& input_server_name,
                                  const AnyValueInputRequest& request) override;

  bool HasAnyValue(const std::string& name) const;

  sup::dto::AnyValue GetAnyValue(const std::string& name) const;

  std::size_t GetSize() const;

  bool WaitForValue(const std::string& name, const sup::dto::AnyValue& value, double seconds) const;

  void SetUserInput(const sup::dto::AnyValue& val);

  bool WaitForInputRequest(const AnyValueInputRequest& request, double seconds) const;

  std::size_t GetNbrInputRequests() const;

  friend std::ostream& operator<<(std::ostream& stream, const TestAnyValueManager& server_if);
private:
  bool HasAnyValueImpl(const std::string& name) const;
  sup::dto::AnyValue GetAnyValueImpl(const std::string& name) const;
  std::map<std::string, sup::dto::AnyValue> m_value_map;
  sup::dto::AnyValue m_user_input;
  std::vector<std::pair<std::string, AnyValueInputRequest>> m_input_requests;
  std::size_t m_n_input_requests;
  mutable std::mutex m_mtx;
  mutable std::condition_variable m_cv;
};

class TestAnyValueManagerRegistry : public IAnyValueManagerRegistry
{
public:
  TestAnyValueManagerRegistry();
  ~TestAnyValueManagerRegistry();

  IAnyValueManager& GetAnyValueManager(std::size_t idx) override;
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
