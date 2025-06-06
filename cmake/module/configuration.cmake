# -----------------------------------------------------------------------------
# Main project configuration
# -----------------------------------------------------------------------------

include(GNUInstallDirs)
include(CTest)
include(COASetupCodacEnvironment)
include(FetchGithubDeps)

# -----------------------------------------------------------------------------
# C++ version
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

# -----------------------------------------------------------------------------
# Variables
# -----------------------------------------------------------------------------
set(LIBVERSION ${CMAKE_PROJECT_VERSION})
set(LIBSOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR})

# -----------------------------------------------------------------------------
# Directories
# -----------------------------------------------------------------------------
if(NOT DEFINED TEST_OUTPUT_DIRECTORY)
  set(TEST_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_bin)
endif()

file(MAKE_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
find_package(sup-utils REQUIRED)
find_package(sup-dto REQUIRED)
find_package(sup-epics REQUIRED)
find_package(oac-tree REQUIRED)
find_package(sup-protocol REQUIRED)
