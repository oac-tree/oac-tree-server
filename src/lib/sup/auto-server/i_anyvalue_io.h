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

#ifndef SUP_AUTO_SERVER_I_ANYVALUE_IO_H_
#define SUP_AUTO_SERVER_I_ANYVALUE_IO_H_

#include <sup/dto/anyvalue.h>

#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace sup
{
namespace auto_server
{
class IAnyValueManager;

/**
 * @brief This is the interface definition of the automation in terms of AnyValues.
 */
class IAnyValueIO
{
public:
  using NameAnyValuePair = std::pair<std::string, sup::dto::AnyValue>;
  using NameAnyValueSet = std::vector<NameAnyValuePair>;

  virtual ~IAnyValueIO();

  /**
   * @brief Add a set of AnyValues with given unique names that need to be managed.
   *
   * @param name_value_set List of pairs of names and AnyValues to manage.
   * @return true when successful. In case of failure, none of the values is assumed to be added.
   */
  virtual bool AddAnyValues(const NameAnyValueSet& name_value_set) = 0;

  /**
   * @brief Add a handler for user input.
   *
   * @param input_server_name Name of the corresponding input server.
   * @return true when successful. In case of failure, no handler is instantiated.
   */
  virtual bool AddInputHandler(const std::string& input_server_name) = 0;
};

/**
 * @brief Get a list of all the names in the list of name/value pairs.
 *
 * @param name_value_set List of name/value pairs.
 * @return List of names.
 */
std::set<std::string> GetNames(const IAnyValueIO::NameAnyValueSet& name_value_set);

/**
 * @brief Get the set of initial AnyValues related to a job. These include the jobstate and
 * variable AnyValues.
 *
 * @param job_prefix Job specific prefix to use for the AnyValue names.
 * @param n_vars Number of variables in the job.
 * @return List of pairs of AnyValue names and initial values.
 */
IAnyValueIO::NameAnyValueSet GetInitialValueSet(const std::string& job_prefix,
                                                     sup::dto::uint32 n_vars);

/**
 * @brief Get the set of AnyValues related to all instructions of a job.
 *
 * @param job_prefix Job specific prefix to use for the AnyValue names.
 * @param n_instr Number of instructions in the job.
 * @return List of pairs of AnyValue names and initial values for all instructions.
 */
IAnyValueIO::NameAnyValueSet GetInstructionValueSet(const std::string& job_prefix,
                                                         sup::dto::uint32 n_instr);

/**
 * @brief ListenerFactoryFunction defines the signature of a factory function that can be injected
 * into other classes and that will be used to create an IAnyValueIO that will forward all its
 * updates to an IAnyValueManager object.
 */
using ListenerFactoryFunction =
  std::function<std::unique_ptr<IAnyValueIO>(IAnyValueManager&)>;

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_ANYVALUE_IO_H_
