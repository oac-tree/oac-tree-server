add_library(sup-auto-server SHARED)
add_library(sup-auto-server::sup-auto-server ALIAS sup-auto-server) # alias for build-tree usage

add_subdirectory(base)
add_subdirectory(epics)

set_target_properties(sup-auto-server PROPERTIES
  VERSION ${LIBVERSION}
  SOVERSION ${LIBSOVERSION}
  OUTPUT_NAME sup-auto-server
)

target_include_directories(sup-auto-server PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
)

find_package(sup-dto REQUIRED)
find_package(sup-epics REQUIRED)
find_package(sequencer REQUIRED)
find_package(sup-protocol REQUIRED)

target_link_libraries(sup-auto-server
  PUBLIC
    sequencer::sequencer
    sup-dto::sup-dto
    sup-protocol::sup-protocol
  PRIVATE
    sup-epics::sup-epics
)

# -- Installation --
install(TARGETS sup-auto-server EXPORT sup-auto-server-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  anyvalue_input_request.h
  anyvalue_io_helper.h
  anyvalue_utils.h
  attribute_info.h
  automation_client.h
  automation_job_interface.h
  automation_protocol_client.h
  automation_protocol_server.h
  automation_server.h
  client_anyvalue_manager.h
  client_job.h
  client_reply_delegator.h
  epics_io_client.h
  exceptions.h
  i_anyvalue_io.h
  i_anyvalue_manager_registry.h
  i_anyvalue_manager.h
  i_job_info_io.h
  i_job_manager.h
  index_generator.h
  input_protocol_client.h
  input_protocol_server.h
  input_request_server.h
  instruction_info.h
  instruction_map.h
  instruction_state.h
  instruction_tree_utils.h
  job_info_value_mapper.h
  job_info.h
  job_utils.h
  job_value_mapper.h
  anyvalue_io_helper.h
  output_entry_helper.h
  output_entry_types.h
  server_job_info_io.h
  server_job.h
  sup_auto_protocol.h
  variable_info.h
  variable_utils.h
  workspace_info.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/auto-server
)
