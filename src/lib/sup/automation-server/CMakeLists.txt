add_library(sup-automation-server SHARED)
add_library(sup-automation-server::sup-automation-server ALIAS sup-automation-server) # alias for build-tree usage

target_compile_features(sup-automation-server PRIVATE cxx_std_11)

set_target_properties(sup-automation-server PROPERTIES
  VERSION ${LIBVERSION}
  SOVERSION ${LIBSOVERSION}
  OUTPUT_NAME sup-automation-server
)

target_include_directories(sup-automation-server PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
)

find_package(sup-dto REQUIRED)
find_package(sup-utils REQUIRED)
target_link_libraries(sup-automation-server
  PUBLIC
    sup-dto::sup-dto
  PRIVATE
    sup-utils::sup-xml)

target_sources(sup-automation-server
  PRIVATE
  dummy.cpp
)

# -- Installation --
install(TARGETS sup-automation-server EXPORT sup-automation-server-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/automation-server
)
