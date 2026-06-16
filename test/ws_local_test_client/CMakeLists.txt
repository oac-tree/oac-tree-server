set(target_name ws-local-test-client)

add_executable(${target_name})
set_target_properties(${target_name} PROPERTIES
  OUTPUT_NAME ws-local-test-client
  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY}
)

target_link_libraries(${target_name}
  PRIVATE
  oac-tree-server
  sup-utils::sup-cli
)

target_sources(${target_name}
  PRIVATE
  main.cpp
  test_job_info_io.cpp
  utils.cpp
)
