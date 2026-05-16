# ==============================================================================
# cmake/Dependencies.cmake — All external dependencies via FetchContent
# ==============================================================================

include(FetchContent)

# Silence FetchContent noise in non-verbose builds
set(FETCHCONTENT_QUIET ON)

# ------------------------------------------------------------------------------
# Criterion — C unit testing framework
# Chosen over: Unity (less expressive), Check (heavier), cmocka (less ergonomic)
# Criterion has: test suites, fixtures, parameterized tests, TAP output,
#                Valgrind integration, and a clean API.
# ------------------------------------------------------------------------------
FetchContent_Declare(
    criterion
    GIT_REPOSITORY https://github.com/Snaipe/Criterion.git
    GIT_TAG        v2.4.2
    GIT_SHALLOW    TRUE
)

# ------------------------------------------------------------------------------
# Raylib — Graphics (only fetched if ALGLANG_BUILD_GRAPHICS is ON)
# ------------------------------------------------------------------------------
if(ALGLANG_BUILD_GRAPHICS)
    set(RAYLIB_VERSION "5.0")
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        ${RAYLIB_VERSION}
        GIT_SHALLOW    TRUE
    )

    # Raylib build options (disable unused raylib extras)
    set(BUILD_EXAMPLES      OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES         OFF CACHE BOOL "" FORCE)
    set(SUPPORT_FILEFORMAT_BMP  ON  CACHE BOOL "" FORCE)
    set(SUPPORT_FILEFORMAT_PNG  ON  CACHE BOOL "" FORCE)
    set(SUPPORT_FILEFORMAT_JPG  ON  CACHE BOOL "" FORCE)
    set(SUPPORT_FILEFORMAT_OGG  ON  CACHE BOOL "" FORCE)
    set(SUPPORT_FILEFORMAT_WAV  ON  CACHE BOOL "" FORCE)
    set(SUPPORT_FILEFORMAT_MP3  ON  CACHE BOOL "" FORCE)
endif()

# ------------------------------------------------------------------------------
# Populate dependencies
# ------------------------------------------------------------------------------
if(ALGLANG_BUILD_TESTS)
    FetchContent_MakeAvailable(criterion)
endif()

if(ALGLANG_BUILD_GRAPHICS)
    FetchContent_MakeAvailable(raylib)
endif()

# ------------------------------------------------------------------------------
# Platform network libraries
# ------------------------------------------------------------------------------
if(ALGLANG_BUILD_NETWORK)
    if(WIN32)
        set(ALGLANG_NET_LIBS ws2_32 CACHE INTERNAL "")
    else()
        set(ALGLANG_NET_LIBS "" CACHE INTERNAL "")  # POSIX sockets are in libc
    endif()
endif()

# ==============================================================================
# cmake/Sanitizers.cmake — ASAN / UBSAN / TSAN
# ==============================================================================
# Usage: alglang_apply_sanitizers(<target>)

function(alglang_apply_sanitizers target)
    if(MSVC)
        return()
    endif()

    set(_san_flags "")

    if(ALGLANG_ENABLE_ASAN)
        list(APPEND _san_flags -fsanitize=address -fno-omit-frame-pointer)
        message(STATUS "[alglang] ASAN enabled on target: ${target}")
    endif()

    if(ALGLANG_ENABLE_UBSAN)
        list(APPEND _san_flags
            -fsanitize=undefined
            -fsanitize=float-divide-by-zero
            -fsanitize=float-cast-overflow
            -fno-sanitize-recover=all
        )
        message(STATUS "[alglang] UBSAN enabled on target: ${target}")
    endif()

    if(ALGLANG_ENABLE_TSAN)
        if(ALGLANG_ENABLE_ASAN)
            message(FATAL_ERROR "ASAN and TSAN cannot be used simultaneously")
        endif()
        list(APPEND _san_flags -fsanitize=thread)
        message(STATUS "[alglang] TSAN enabled on target: ${target}")
    endif()

    if(_san_flags)
        target_compile_options(${target} PRIVATE ${_san_flags})
        target_link_options(${target}    PRIVATE ${_san_flags})
    endif()
endfunction()

# ==============================================================================
# cmake/Coverage.cmake — gcov / llvm-cov
# ==============================================================================
# Usage: alglang_apply_coverage(<target>)

function(alglang_apply_coverage target)
    if(NOT ALGLANG_ENABLE_COVERAGE)
        return()
    endif()
    if(MSVC)
        message(WARNING "[alglang] Coverage not supported with MSVC")
        return()
    endif()

    target_compile_options(${target} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(${target}    PRIVATE --coverage)

    # Custom target to generate HTML report (requires lcov + genhtml)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    if(LCOV_PATH AND GENHTML_PATH)
        add_custom_target(coverage
            COMMAND ${LCOV_PATH} --directory . --capture
                    --output-file coverage.info
                    --exclude "*/tests/*" --exclude "*/third_party/*"
            COMMAND ${GENHTML_PATH} coverage.info
                    --output-directory coverage_html
                    --title "AlgLang Coverage"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "Generating coverage HTML report in ${CMAKE_BINARY_DIR}/coverage_html"
            VERBATIM
        )
    endif()
endfunction()

# ==============================================================================
# cmake/CompilerWarnings.cmake — documented here for reference
# (actual flags are in the INTERFACE target in root CMakeLists.txt)
# ==============================================================================
# All warning flags are applied through the alglang_compile_options INTERFACE
# target defined in the root CMakeLists.txt. This file is a placeholder
# for future per-compiler tuning if needed.

# ==============================================================================
# cmake/version.h.in — Generated version header
# ==============================================================================
# NOTE: This file is configured by configure_file() in root CMakeLists.txt
# Content:
#
# #pragma once
# #define ALGLANG_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
# #define ALGLANG_VERSION_MINOR @PROJECT_VERSION_MINOR@
# #define ALGLANG_VERSION_PATCH @PROJECT_VERSION_PATCH@
# #define ALGLANG_VERSION_STRING "@PROJECT_VERSION@"

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

# ==============================================================================
# CMakePresets.json content (save as CMakePresets.json in project root)
# ==============================================================================
#
# {
#   "version": 6,
#   "configurePresets": [
#     {
#       "name": "base",
#       "hidden": true,
#       "generator": "Ninja",
#       "binaryDir": "${sourceDir}/build/${presetName}",
#       "cacheVariables": {
#         "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
#       }
#     },
#     {
#       "name": "debug",
#       "displayName": "Debug",
#       "inherits": "base",
#       "cacheVariables": {
#         "CMAKE_BUILD_TYPE": "Debug",
#         "ALGLANG_BUILD_TESTS": "ON",
#         "ALGLANG_BUILD_TOOLS": "ON"
#       }
#     },
#     {
#       "name": "release",
#       "displayName": "Release",
#       "inherits": "base",
#       "cacheVariables": {
#         "CMAKE_BUILD_TYPE": "Release",
#         "ALGLANG_USE_LTO": "ON",
#         "ALGLANG_BUILD_TESTS": "OFF",
#         "ALGLANG_WARNINGS_AS_ERRORS": "ON"
#       }
#     },
#     {
#       "name": "asan",
#       "displayName": "Debug + ASAN + UBSAN",
#       "inherits": "debug",
#       "cacheVariables": {
#         "ALGLANG_ENABLE_ASAN": "ON",
#         "ALGLANG_ENABLE_UBSAN": "ON"
#       }
#     },
#     {
#       "name": "coverage",
#       "displayName": "Debug + Coverage",
#       "inherits": "debug",
#       "cacheVariables": {
#         "ALGLANG_ENABLE_COVERAGE": "ON"
#       }
#     },
#     {
#       "name": "no-graphics",
#       "displayName": "Debug without graphics",
#       "inherits": "debug",
#       "cacheVariables": {
#         "ALGLANG_BUILD_GRAPHICS": "OFF"
#       }
#     }
#   ],
#   "buildPresets": [
#     { "name": "debug",    "configurePreset": "debug"    },
#     { "name": "release",  "configurePreset": "release"  },
#     { "name": "asan",     "configurePreset": "asan"     },
#     { "name": "coverage", "configurePreset": "coverage" }
#   ],
#   "testPresets": [
#     {
#       "name": "unit",
#       "configurePreset": "debug",
#       "filter": { "include": { "label": "unit" } },
#       "output": { "verbosity": "verbose" }
#     },
#     {
#       "name": "all",
#       "configurePreset": "debug",
#       "output": { "outputOnFailure": true }
#     }
#   ]
# }
