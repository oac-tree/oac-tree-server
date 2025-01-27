/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - OAC-TREE-SERVER
 *
 * Description   : oac-tree server
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/oac-tree-server/index_generator.h>

namespace sup
{
namespace oac_tree_server
{

IndexGenerator::IndexGenerator()
  : m_last_idx{0}
  , m_mtx{}
{}

IndexGenerator::~IndexGenerator() = default;

sup::dto::uint64 IndexGenerator::CurrentIndex()
{
  std::lock_guard<std::mutex> lk{m_mtx};
  return m_last_idx;
}

sup::dto::uint64 IndexGenerator::NewIndex()
{
  std::lock_guard<std::mutex> lk{m_mtx};
  if (++m_last_idx == 0)
  {
    ++m_last_idx;
  }
  return m_last_idx;
}

}  // namespace oac_tree_server

}  // namespace sup
