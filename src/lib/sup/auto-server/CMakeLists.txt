add_library(sup-auto-server SHARED)
add_library(sup-auto-server::sup-auto-server ALIAS sup-auto-server) # alias for build-tree usage

target_compile_features(sup-auto-server PRIVATE cxx_std_11)

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
target_link_libraries(sup-auto-server
  PUBLIC
    sequencer::sequencer
    sup-dto::sup-dto
  PRIVATE
    sup-epics::sup-epics
)

# -- Installation --
install(TARGETS sup-auto-server EXPORT sup-auto-server-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  anyvalue_manager_interface.h
  attribute_info.h
  automation_job_interface.h
  automation_server_protocol.h
  automation_server.h
  client_interface_mapper.h
  exceptions.h
  instruction_info.h
  instruction_proxy.h
  old_job_info.h
  job_proxy.h
  job_value_mapper.h
  job.h
  sup_auto_protocol.h
  variable_info.h
  variable_proxy.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/auto-server
)
