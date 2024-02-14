add_library(sup-auto-server SHARED)
add_library(sup-auto-server::sup-auto-server ALIAS sup-auto-server) # alias for build-tree usage

target_compile_features(sup-auto-server PRIVATE cxx_std_11)

add_subdirectory(base)

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
find_package(sequencer REQUIRED)
target_link_libraries(sup-auto-server
  PUBLIC
    sequencer::sequencer
    sup-dto::sup-dto
)

# -- Installation --
install(TARGETS sup-auto-server EXPORT sup-auto-server-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/auto-server
)
