add_library(oac-tree-server-shared SHARED)
add_library(oac-tree-server-shared::oac-tree-server-shared ALIAS oac-tree-server-shared) # alias for build-tree usage

add_subdirectory(base)
add_subdirectory(epics)

set_target_properties(oac-tree-server-shared PROPERTIES
  VERSION ${LIBVERSION}
  SOVERSION ${LIBSOVERSION}
  OUTPUT_NAME oac-tree-server
)

target_include_directories(oac-tree-server-shared PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
)

target_link_libraries(oac-tree-server-shared
  PUBLIC
    oac-tree::oac-tree
    sup-dto::sup-dto
    sup-protocol::sup-protocol
  PRIVATE
    sup-epics::sup-epics
)

# -- Installation --
install(TARGETS oac-tree-server-shared EXPORT oac-tree-server-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  anyvalue_io_helper.h
  automation_client_stack.h
  automation_protocol_client.h
  automation_server.h
  client_anyvalue_manager.h
  client_job.h
  client_reply_delegator.h
  control_protocol_server.h
  epics_config_utils.h
  exceptions.h
  i_anyvalue_io.h
  i_anyvalue_manager_registry.h
  i_anyvalue_manager.h
  i_job_manager.h
  index_generator.h
  info_protocol_server.h
  input_protocol_client.h
  input_protocol_server.h
  input_reply_helper.h
  input_request_helper.h
  input_request_server.h
  oac_tree_protocol.h
  output_entry_helper.h
  output_entry_types.h
  server_job_info_io.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/oac-tree-server
)
