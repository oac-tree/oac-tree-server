# -----------------------------------------------------------------------------
# Installation
# Credits to https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right,
# https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/Exporting-and-Importing-Targets
# -----------------------------------------------------------------------------

include(PackageConfig)
include(CMakePackageConfigHelpers)

set(BUILD_CONFIGDIR ${CMAKE_BINARY_DIR})
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/oac-tree-server-shared)

# -----------------------------------------------------------------------------
# Exporting targets
# -----------------------------------------------------------------------------
set(PACKAGE_TARGETS_FILENAME oac-tree-server-targets.cmake)
set(PACKAGE_TARGETS_FILE ${BUILD_CONFIGDIR}/${PACKAGE_TARGETS_FILENAME})

# Install the export targets for installation usage, this does not relate to the build tree file
install(EXPORT oac-tree-server-targets FILE ${PACKAGE_TARGETS_FILENAME} NAMESPACE oac-tree-server:: DESTINATION ${INSTALL_CONFIGDIR})

# Generate the export targets for the build tree usage
export(TARGETS oac-tree-server-shared NAMESPACE oac-tree-server:: FILE ${PACKAGE_TARGETS_FILE})

# Export the package to CMake registry for build tree usage (goes to $HOME/.cmake)
if(COA_EXPORT_BUILD_TREE)
  set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
  export(PACKAGE oac-tree-server-shared)
endif()

# -----------------------------------------------------------------------------
# Version configuration
# -----------------------------------------------------------------------------
set(PACKAGE_VERSION_FILE ${BUILD_CONFIGDIR}/oac-tree-server-config-version.cmake)

# Generate the version config file, shared in both build tree and installation usage
write_basic_package_version_file(${PACKAGE_VERSION_FILE} COMPATIBILITY AnyNewerVersion)

install(FILES ${PACKAGE_VERSION_FILE} DESTINATION ${INSTALL_CONFIGDIR})

# -----------------------------------------------------------------------------
# Package configuration
# -----------------------------------------------------------------------------
set(PACKAGE_CONFIG_FILE ${BUILD_CONFIGDIR}/oac-tree-server-config.cmake)

# Generate the package config file, shared in both build tree and installation usage
write_package_config_file(oac-tree-server-shared OUTPUT ${PACKAGE_CONFIG_FILE} INSTALL_DESTINATION ${INSTALL_CONFIGDIR} DEPENDENCIES oac-tree sup-protocol)

install(FILES ${PACKAGE_CONFIG_FILE} DESTINATION ${INSTALL_CONFIGDIR})
