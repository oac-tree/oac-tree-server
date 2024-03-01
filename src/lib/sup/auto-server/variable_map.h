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

#ifndef SUP_AUTO_SERVER_VARIABLE_MAP_H_
#define SUP_AUTO_SERVER_VARIABLE_MAP_H_

#include <sup/dto/anyvalue.h>

#include <map>
#include <string>

namespace sup
{
namespace sequencer
{
class Workspace;
}  // namespace sequencer

namespace auto_server
{
/**
 * @brief VariableMap builds a map from variable names to a unique index. The use
 * of indexed identifiers instead of the variable names themselves prevents possible issues with
 * forbidden characters in the channel names.
 *
 * @note This class is immutable and builds all the necessary structures during construction. This
 * allows to use VariableMap objects easily in a multithreaded context.
 */
class VariableMap
{
public:
  /**
   * @brief Construct a VariableMap object from the given Workspace.
   *
   * @param workspace Workspace of variables to map.
   */
  explicit VariableMap(const sequencer::Workspace& workspace);

  /**
   * @brief Destructor.
   */
  ~VariableMap();

  /**
   * @brief Find the variable index that will be used as part of the channel name from its
   * name.
   *
   * @param var_name Variable name.
   * @return Variable index.
   *
   * @throw InvalidOperationException when variable name is not known.
   */
  sup::dto::uint32 FindVariableIndex(const std::string& var_name) const;

  /**
   * @brief Return the map of variable names to their indices.
   *
   * @return map of variable names to variable indices.
   */
  std::map<std::string, sup::dto::uint32> GetVariableMapping() const;

  /**
   * @brief Return the number of variables that are mapped.
   *
   * @return Number of variables mapped.
   */
  sup::dto::uint32 GetNumberOfVariables() const;

private:
  /**
   * @brief Initialization member function, called only during construction.
   *
   * @param workspace Workspace whose variables will be mapped.
   */
  void InitializeMap(const sequencer::Workspace& workspace);
  std::map<std::string, sup::dto::uint32> m_variable_map;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_VARIABLE_MAP_H_
