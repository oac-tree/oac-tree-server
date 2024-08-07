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

#ifndef SUP_AUTO_SERVER_I_ANYVALUE_MANAGER_H_
#define SUP_AUTO_SERVER_I_ANYVALUE_MANAGER_H_

#include <sup/dto/anyvalue.h>

#include <set>
#include <string>
#include <vector>
#include <utility>

namespace sup
{
namespace auto_server
{

/**
 * @brief IAnyValueManager defines the API for implementations that manage multiple sets of
 * (name, value) pairs, where value is encoded as an AnyValue.
 *
 * @details Implementations of this interface will be used on the server side to publish updates of
 * instructions, variables, etc. On the client side, the implementation will be used to map such
 * updates to the appropriate JobInterface methods.
 *
 * @todo Provide the API for client input and message queues, i.e. values whose history may matter.
 */
class IAnyValueManager
{
public:
  using NameAnyValuePair = std::pair<std::string, sup::dto::AnyValue>;
  using NameAnyValueSet = std::vector<NameAnyValuePair>;

  virtual ~IAnyValueManager();

  /**
   * @brief Add a set of AnyValues with given unique names.
   *
   * @details Typical implementations should be able to support multiple calls to this member
   * function, allowing to manage additional sets of AnyValues in different stages of setting up
   * the client or server application. In the case of Sequencer procedures on the server side, this
   * is required since the job and variable states need to be present before procedure setup, while
   * the instruction states can only be created afterwards.
   *
   * @param name_value_set List of pairs of names and AnyValues.
   * @return true when successful. In case of failure, none of the values is assumed to be added.
   */
  virtual bool AddAnyValues(const NameAnyValueSet& name_value_set) = 0;

  /**
   * @brief Update the value of the managed AnyValue with the given name.
   *
   * @param name Name of the managed AnyValue to be updated.
   * @param value New value for the managed AnyValue.
   * @return true on success. Failure may include the case of an unknown name.
   */
  virtual bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) = 0;
};

/**
 * @brief Get a list of all the names in the list of name/value pairs.
 *
 * @param name_value_set List of name/value pairs.
 * @return List of names.
 */
std::set<std::string> GetNames(const IAnyValueManager::NameAnyValueSet& name_value_set);

/**
 * @brief Get the set of initial AnyValues related to a job. These include the jobstate and
 * variable AnyValues.
 *
 * @param job_prefix Job specific prefix to use for the AnyValue names.
 * @param n_vars Number of variables in the job.
 * @return List of pairs of AnyValue names and initial values.
 */
IAnyValueManager::NameAnyValueSet GetInitialValueSet(const std::string& job_prefix,
                                                     sup::dto::uint32 n_vars);

/**
 * @brief Get the set of AnyValues related to all instructions of a job.
 *
 * @param job_prefix Job specific prefix to use for the AnyValue names.
 * @param n_instr Number of instructions in the job.
 * @return List of pairs of AnyValue names and initial values for all instructions.
 */
IAnyValueManager::NameAnyValueSet GetInstructionValueSet(const std::string& job_prefix,
                                                         sup::dto::uint32 n_instr);

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_ANYVALUE_MANAGER_H_
