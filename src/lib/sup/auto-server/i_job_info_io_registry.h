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

#ifndef SUP_AUTO_SERVER_I_JOB_INFO_IO_REGISTRY_H_
#define SUP_AUTO_SERVER_I_JOB_INFO_IO_REGISTRY_H_

#include <sup/auto-server/i_job_info_io.h>

namespace sup
{
namespace auto_server
{

  /**
 * @brief Interface for classes that provide IJobInfoIO implementations and manage
 * their lifecycle.
 */
class IJobInfoIORegistry
{
public:
  virtual ~IJobInfoIORegistry();

  /**
   * @brief Get a reference to an IJobInfoIO object.
   *
   * @param idx Index that identifies which object to return.
   * @return Reference to an IJobInfoIO implementation.
   */
  virtual IJobInfoIO& GetJobInfoIO(std::size_t idx) = 0;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_JOB_INFO_IO_REGISTRY_H_
