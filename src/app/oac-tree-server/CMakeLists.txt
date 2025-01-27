set(target_name oac-tree-server-exec)

add_executable(${target_name})
set_target_properties(${target_name} PROPERTIES OUTPUT_NAME oac-tree-server)

target_link_libraries(${target_name}
  PRIVATE
  oac-tree-server
  sup-epics::sup-epics
  sup-utils::sup-cli
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1))
message(VERBOSE "Linking stdc++fs")
target_link_libraries(${target_name} PRIVATE stdc++fs)
endif ()

target_sources(${target_name}
  PRIVATE
  main.cpp
  utils.cpp
)

# -- Installation --
install(TARGETS ${target_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
