# Collection of functions used for handling dependencies

if(NOT COA_FETCH_DEPS)
  return()
endif()

message(STATUS "Building dependencies from source")

include(FetchContent)

FetchContent_Declare(
  sup-protocol
  GIT_REPOSITORY https://github.com/oac-tree/sup-protocol.git
  GIT_TAG develop
  OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
  sup-epics
  GIT_REPOSITORY https://github.com/oac-tree/sup-epics.git
  GIT_TAG develop
  OVERRIDE_FIND_PACKAGE
)
