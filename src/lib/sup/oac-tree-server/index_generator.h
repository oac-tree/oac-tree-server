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
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_OAC_TREE_SERVER_INDEX_GENERATOR_H_
#define SUP_OAC_TREE_SERVER_INDEX_GENERATOR_H_

#include <sup/dto/basic_scalar_types.h>

#include <mutex>

namespace sup
{
namespace oac_tree_server
{

/**
 * @brief Class that generates non-zero indices consecutively in a threadsafe manner.
 *
 * @note In the unlikely case that the index wraps around, an index of 1 will be returned (never
 * zero).
 */
class IndexGenerator
{
public:
  IndexGenerator();
  ~IndexGenerator();

  sup::dto::uint64 CurrentIndex();

  sup::dto::uint64 NewIndex();
private:
  sup::dto::uint64 m_last_idx;
  mutable std::mutex m_mtx;
};

}  // namespace oac_tree_server

}  // namespace sup

#endif  // SUP_OAC_TREE_SERVER_INDEX_GENERATOR_H_
