if(NOT COA_PARASOFT_INTEGRATION)
  return()
endif()

include(COASetupParasoft)

# Add parasoft project for sup-auto-server-target
# This generates the build time information needed by Parasoft for analysis
cpptest_add_executable(oac-tree-server-cpptest
  CPPTEST_PROJECT_LOC ${CMAKE_CURRENT_BINARY_DIR}
  CPPTEST_PROJECT_FOLDERS
     src-oac-tree-server=${CMAKE_SOURCE_DIR}/src/lib/sup/oac-tree-server/
  TARGETS oac-tree-server
)

get_target_property(OAC_TREE_SERVER_INCLUDE_DIRECTORIES oac-tree-server INCLUDE_DIRECTORIES)
target_include_directories(oac-tree-server-cpptest PUBLIC ${OAC_TREE_SERVER_INCLUDE_DIRECTORIES})

target_link_libraries(oac-tree-server-cpptest
  PUBLIC
  sup-dto::sup-dto
  sup-utils::sup-cli
  sup-protocol::sup-protocol
  sup-epics::sup-epics
  oac-tree::oac-tree
  oac-tree-server
)
