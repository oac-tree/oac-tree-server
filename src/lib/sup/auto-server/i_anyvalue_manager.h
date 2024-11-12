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

#include <sup/auto-server/i_anyvalue_io.h>
#include <sup/auto-server/input_request_helper.h>

#include <sup/sequencer/user_input_reply.h>
#include <sup/sequencer/user_input_request.h>

namespace sup
{
namespace auto_server
{

using sup::sequencer::UserInputReply;
using sup::sequencer::UserInputRequest;

/**
 * @brief IAnyValueManager defines an additional API for updates to managed AnyValues and to
 * handle user input.
 *
 * @details Implementations of this interface will be used on the server side to publish updates of
 * instructions, variables, etc. On the client side, the implementation will be used to map such
 * updates to the appropriate JobInterface methods.
 */
class IAnyValueManager : public IAnyValueIO
{
public:

  virtual ~IAnyValueManager();

  /**
   * @brief Update the value of the managed AnyValue with the given name.
   *
   * @param name Name of the managed AnyValue to be updated.
   * @param value New value for the managed AnyValue.
   * @return true on success. Failure may include the case of an unknown name.
   */
  virtual bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) = 0;

  /**
   * @brief Get user input using the given input server and request information.
   *
   * @param input_server_name Name of the input server.
   * @param id Identification of the user input request.
   * @param request Description of the input requested.
   * @return AnyValue response from the user.
   */
  virtual UserInputReply GetUserInput(const std::string& input_server_name, sup::dto::uint64 id,
                                      const UserInputRequest& request) = 0;

  /**
   * @brief Interrupt a user input request.
   *
   * @note Due to multithreading, an interrupt request with a specific id may be triggered before
   * the corresponding input method was called. Implementations need to take care of this case.
   *
   * @param input_server_name Name of the input server.
   * @param id Identification of the user input request to interrupt.
   */
  virtual void Interrupt(const std::string& input_server_name, sup::dto::uint64 id) = 0;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_I_ANYVALUE_MANAGER_H_
