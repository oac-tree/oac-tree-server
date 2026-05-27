# Project       : SUP network with ASIO
#
# Description   : Network ASIO-based utilities for Supervision and Automation
#
# Author        : Gennady Pospelov (IO)
#
# Copyright (c) : 2010-2025 ITER Organization,
#                 CS 90 046
#                 13067 St. Paul-lez-Durance Cedex
#                 France
# SPDX-License-Identifier: MIT
#
# This file is part of ITER CODAC software.
# For the terms and conditions of redistribution or use of this software
# refer to the file LICENSE located in the top level directory
# of the distribution package.

find_package(PkgConfig)
pkg_check_modules(PC_Asio QUIET asio)

find_path(Asio_INCLUDE_DIR NAMES asio.hpp PATHS ${PC_Asio_INCLUDE_DIRS} PATH_SUFFIXES asio)

set(Asio_VERSION ${PC_Asio_VERSION})

mark_as_advanced(Asio_FOUND Asio_INCLUDE_DIR Asio_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Asio REQUIRED_VARS Asio_INCLUDE_DIR VERSION_VAR Asio_VERSION)

if(Asio_FOUND)
  set(Asio_INCLUDE_DIRS ${Asio_INCLUDE_DIR})
endif()

if(Asio_FOUND AND NOT TARGET Asio::Asio)
  add_library(Asio::Asio INTERFACE IMPORTED)
  set_target_properties(Asio::Asio PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${Asio_INCLUDE_DIR}")
endif()
