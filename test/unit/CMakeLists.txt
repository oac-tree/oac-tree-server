add_executable(unit-tests)

target_compile_features(unit-tests PRIVATE cxx_std_11)

target_sources(unit-tests
  PRIVATE
    # epics_instruction_pv_handler_tests.cpp
    # epics_job_pv_handler_tests.cpp
    instruction_tree_cache_tests.cpp
    job_value_mapper_tests.cpp
    # server_ui_tests.cpp
    unit_test_helper.cpp
)

target_include_directories(unit-tests
  PRIVATE
)

find_package(GTest REQUIRED)
find_package(sup-epics)

include(GoogleTest)

target_link_libraries(unit-tests
  PRIVATE
    GTest::GTest
    GTest::Main
    sup-auto-server
    sup-epics::sup-epics
)

set_target_properties(unit-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

gtest_discover_tests(unit-tests)
