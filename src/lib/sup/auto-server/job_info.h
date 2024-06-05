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

#ifndef SUP_AUTO_SERVER_JOB_INFO_H_
#define SUP_AUTO_SERVER_JOB_INFO_H_

#include <sup/sequencer/procedure.h>

#include <string>

namespace sup
{
namespace auto_server
{
/**
 * @brief The JobInfo.
 */
class JobInfo
{
public:
  JobInfo(const std::string& prefix, const sup::sequencer::Procedure& proc);
  ~JobInfo();

  void SetInstructionTreeInfo(const sup::dto::AnyValue& instr_tree_info);

  std::string GetPrefix() const;
  std::string GetProcedureName() const;
  std::size_t GetNumberOfVariables() const;
  sup::dto::AnyValue GetInstructionTreeInfo() const;

private:
  std::string m_job_prefix;
  std::string m_full_name;
  std::size_t m_nr_vars;
  sup::dto::AnyValue m_instr_tree_info;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_JOB_INFO_H_
