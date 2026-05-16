# ==============================================================================
# tests/CMakeLists.txt — Test suite
# ==============================================================================
# This file is meant to be placed at tests/CMakeLists.txt

# Collect all unit test sources
file(GLOB UNIT_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/test_*.c"
)

# Collect all integration test sources
file(GLOB INTEGRATION_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/integration/test_*.c"
)

# Build one test executable per unit test file (fast, isolated failures)
foreach(_test_src ${UNIT_TEST_SOURCES})
    get_filename_component(_test_name ${_test_src} NAME_WE)

    add_executable(${_test_name} ${_test_src}
        "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/test_helpers.c"
    )

    target_link_libraries(${_test_name}
        PRIVATE
            alglang_core
            alglang_compile_options
            criterion          # Criterion test framework
    )

    target_include_directories(${_test_name}
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
            "${CMAKE_SOURCE_DIR}/src"
            "${CMAKE_SOURCE_DIR}/include"
            "${CMAKE_BINARY_DIR}/include"
    )

    alglang_apply_sanitizers(${_test_name})
    alglang_apply_coverage(${_test_name})

    # Register with CTest
    add_test(
        NAME    ${_test_name}
        COMMAND ${_test_name} --verbose
    )

    set_tests_properties(${_test_name} PROPERTIES
        TIMEOUT         30
        LABELS          "unit"
        ENVIRONMENT     "ALGLANG_LANG_PATH=${CMAKE_SOURCE_DIR}/languages"
    )
endforeach()

# Integration tests: one executable, each .alg file is a test case
if(INTEGRATION_TEST_SOURCES)
    add_executable(alglang_integration_tests
        ${INTEGRATION_TEST_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/test_helpers.c"
    )

    target_link_libraries(alglang_integration_tests
        PRIVATE
            alglang_core
            alglang_compile_options
            criterion
    )

    target_include_directories(alglang_integration_tests
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
            "${CMAKE_SOURCE_DIR}/src"
            "${CMAKE_SOURCE_DIR}/include"
            "${CMAKE_BINARY_DIR}/include"
    )

    # Pass the test programs directory and alglang binary path to tests
    target_compile_definitions(alglang_integration_tests PRIVATE
        TEST_PROGRAMS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/integration/programs"
        TEST_EXPECTED_DIR="${CMAKE_CURRENT_SOURCE_DIR}/integration/expected"
        ALGLANG_BINARY="$<TARGET_FILE:alglang>"
        ALGLANG_LANG_PATH="${CMAKE_SOURCE_DIR}/languages"
    )

    alglang_apply_sanitizers(alglang_integration_tests)

    add_test(
        NAME    alglang_integration
        COMMAND alglang_integration_tests --verbose
    )

    set_tests_properties(alglang_integration PROPERTIES
        TIMEOUT         120
        LABELS          "integration"
    )
endif()

# Convenience target: run only unit tests
add_custom_target(test_unit
    COMMAND ${CMAKE_CTEST_COMMAND} -L unit --output-on-failure
    DEPENDS ${UNIT_TEST_NAMES}
    COMMENT "Running unit tests"
)

# Convenience target: run all tests with verbose output
add_custom_target(test_all
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -j4
    COMMENT "Running all tests"
)
