add_executable(unit-tests)

target_sources(unit-tests
  PRIVATE
    anyvalue_input_request_tests.cpp
    app_utils_tests.cpp
    automation_server_tests.cpp
    client_anyvalue_manager_tests.cpp
    epics_anyvalue_manager_tests.cpp
    epics_client_server_tests.cpp
    index_generator_tests.cpp
    input_request_server_tests.cpp
    job_info_io_server_client_tests.cpp
    output_entry_tests.cpp
    sup_auto_protocol_tests.cpp
    unit_test_helper.cpp
    ../../src/app/sup-automation-server/utils.cpp
)

target_include_directories(unit-tests
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../../src
)

find_package(GTest REQUIRED)
find_package(sup-utils REQUIRED)
find_package(sup-epics REQUIRED)

include(GoogleTest)

target_link_libraries(unit-tests
  PRIVATE
    GTest::GTest
    GTest::Main
    GTest::gmock
    sup-auto-server
    sup-epics::sup-epics
    sup-utils::sup-cli
    stdc++fs
)

set_target_properties(unit-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

gtest_discover_tests(unit-tests)
