set(unit-tests oac-tree-server-unit-tests)
add_executable(${unit-tests})

set_target_properties(${unit-tests} PROPERTIES OUTPUT_NAME "unit-tests")
set_target_properties(${unit-tests} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

target_sources(${unit-tests}
  PRIVATE
    anyvalue_io_helper_tests.cpp
    anyvalue_update_command_tests.cpp
    anyvalue_update_queue_tests.cpp
    app_utils_tests.cpp
    automation_server_tests.cpp
    client_anyvalue_manager_tests.cpp
    client_job_tests.cpp
    client_reply_delegator_tests.cpp
    epics_anyvalue_manager_tests.cpp
    epics_client_server_tests.cpp
    epics_input_client_server_tests.cpp
    full_client_server_stack_tests.cpp
    index_generator_tests.cpp
    input_protocol_client_server_tests.cpp
    input_reply_helper_tests.cpp
    input_request_helper_tests.cpp
    input_request_server_tests.cpp
    job_info_io_server_client_tests.cpp
    job_manager_client_server_stack_tests.cpp
    oac_tree_protocol_tests.cpp
    output_entry_tests.cpp
    protocol_client_server_tests.cpp
    unit_test_helper.cpp
    ../../src/app/oac-tree-server/utils.cpp
)

target_include_directories(${unit-tests}
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../../src
)

find_package(GTest REQUIRED)
find_package(sup-utils REQUIRED)
find_package(sup-epics REQUIRED)

include(GoogleTest)

target_link_libraries(${unit-tests}
  PRIVATE
    GTest::GTest
    GTest::Main
    GTest::gmock
    oac-tree-server
    sup-epics::sup-epics
    sup-utils::sup-cli
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1))
message(VERBOSE "Linking stdc++fs")
target_link_libraries(${unit-tests} PRIVATE stdc++fs)
endif ()

gtest_discover_tests(${unit-tests} DISCOVERY_MODE PRE_TEST)
