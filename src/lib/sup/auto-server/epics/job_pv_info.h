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

#ifndef SUP_AUTO_SERVER_JOB_PV_INFO_H_
#define SUP_AUTO_SERVER_JOB_PV_INFO_H_

#include <sup/dto/anyvalue.h>

#include <string>

namespace sup
{

namespace auto_server
{

/**
 * @brief JobPVInfo collects the required information to create the PvAccess server PVs for a
 * single job.
 */
class JobPVInfo
{
public:
  JobPVInfo(const std::string& prefix, const sup::dto::AnyValue& instr_tree,
            sup::dto::uint32 n_variables);
  ~JobPVInfo();

  std::string GetJobStateChannel() const;
  std::string GetInstructionTreeChannel() const;

  sup::dto::uint32 GetNumberOfVariables() const;
  std::string GetVariableChannel(sup::dto::uint32 index) const;

  sup::dto::AnyValue GetInstructionTreeStructure() const;

private:
  const std::string m_prefix;
  const sup::dto::AnyValue m_instr_tree;
  const sup::dto::uint32 m_n_variables;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_PV_INFO_H_
