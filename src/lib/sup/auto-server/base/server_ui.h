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

#ifndef SUP_AUTO_SERVER_SERVER_UI_H_
#define SUP_AUTO_SERVER_SERVER_UI_H_

#include <sup/sequencer/user_interface.h>

namespace sup
{
namespace auto_server
{

class ServerUserInterface : public sequencer::UserInterface
{
public:
  ServerUserInterface();

  ~ServerUserInterface();

  void UpdateInstructionStatus(const sequencer::Instruction* instruction) override;
  void VariableUpdated(const std::string& name, const sup::dto::AnyValue& value, bool connected) override;
  bool PutValue(const sup::dto::AnyValue& value, const std::string& description) override;
  bool GetUserValue(sup::dto::AnyValue& value, const std::string& description) override;
  int GetUserChoice(const std::vector<std::string>& options,
                            const sup::dto::AnyValue& metadata) override;
  void Message(const std::string& message) override;
  void Log(int severity, const std::string& message) override;
};

}  // namespace auto_server

}  // namespace sup

#endif  // SUP_AUTO_SERVER_SERVER_UI_H_
