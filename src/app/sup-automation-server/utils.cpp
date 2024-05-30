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

#include "utils.h"

#include <sup/sequencer/sequence_parser.h>

namespace sup
{
namespace auto_server
{
namespace utils
{

const std::string kSimpleCounterProcedureString{
R"RAW(<?xml version="1.0" encoding="UTF-8"?>
<Procedure>
  <Plugin>libsequencer-pvxs.so</Plugin>
  <Repeat maxCount="-1">
    <Sequence>
      <Wait timeout="1"/>
      <Increment varName="counter"/>
      <Copy inputVar="counter" outputVar="server.value"/>
    </Sequence>
  </Repeat>
  <Workspace>
    <PvAccessServer name="server"
                    channel="TEST:SERVER-COUNTER"
                    type='{"type":"","attributes":[{"value":{"type":"uint64"}}]}'
                    value='{"value":0}'/>
    <Local name="counter" type='{"type":"uint64"}' value='0' />
  </Workspace>
</Procedure>
)RAW"};

AutomationServer::ProcedureList GetProcedureList(sup::cli::CommandLineParser& parser)
{
  (void)parser;
  AutomationServer::ProcedureList result;
  result.emplace_back(sup::sequencer::ParseProcedureString(kSimpleCounterProcedureString));
  return result;
}

}  // namespace utils

}  // namespace auto_server

}  // namespace sup
