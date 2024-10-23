add_executable(sup-automation-server)

find_package(sup-utils REQUIRED)
find_package(sup-epics REQUIRED)

target_link_libraries(sup-automation-server
  PRIVATE
  sup-auto-server
  sup-epics::sup-epics
  sup-utils::sup-cli
  stdc++fs
)

target_sources(sup-automation-server
  PRIVATE
  main.cpp
  utils.cpp
)

# -- Installation --
install(TARGETS sup-automation-server RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
