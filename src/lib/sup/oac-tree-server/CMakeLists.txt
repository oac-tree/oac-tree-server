add_library(oac-tree-server SHARED)
add_library(oac-tree-server::oac-tree-server ALIAS oac-tree-server) # alias for build-tree usage

add_subdirectory(base)
add_subdirectory(epics)
add_subdirectory(websocket)

set_target_properties(oac-tree-server PROPERTIES
  VERSION ${LIBVERSION}
  SOVERSION ${LIBSOVERSION}
  OUTPUT_NAME oac-tree-server
)

target_include_directories(oac-tree-server PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
)

target_include_directories(oac-tree-server SYSTEM PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../../thirdparty
)

target_compile_definitions(oac-tree-server PRIVATE RAPIDJSON_HAS_STDSTRING ASIO_STANDALONE)

target_link_libraries(oac-tree-server
  PUBLIC
    oac-tree::oac-tree
    sup-dto::sup-dto
    sup-protocol::sup-protocol
  PRIVATE
    sup-epics::sup-epics
    Asio::Asio
    OpenSSL::Crypto
)

# -- Installation --
install(TARGETS oac-tree-server EXPORT oac-tree-server-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  automation_server.h
  epics_utils.h
  exceptions.h
  i_client_job_manager.h
  i_job_manager.h
  i_server_protocol_factory.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/oac-tree-server
)
