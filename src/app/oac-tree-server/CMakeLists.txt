add_executable(oac-tree-server)

target_link_libraries(oac-tree-server
  PRIVATE
  oac-tree-server-shared
  sup-epics::sup-epics
  sup-utils::sup-cli
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1))
message(VERBOSE "Linking stdc++fs")
target_link_libraries(oac-tree-server PRIVATE stdc++fs)
endif ()

target_sources(oac-tree-server
  PRIVATE
  main.cpp
  utils.cpp
)

# -- Installation --
install(TARGETS oac-tree-server RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
