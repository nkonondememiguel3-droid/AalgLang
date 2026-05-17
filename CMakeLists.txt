cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(
    alglang
    VERSION      0.1.0
    DESCRIPTION  "Human-language-agnostic algorithm language compiler and VM"
    HOMEPAGE_URL "https://github.com:nkonondememiguel3-droid/AalgLang.git"
    LANGUAGES    C
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_C_STANDARD            11)
set(CMAKE_C_STANDARD_REQUIRED   ON)
set(CMAKE_C_EXTENSIONS          OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "[alglang] No build type specified — defaulting to Debug")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

option(ALGLANG_BUILD_TESTS      "Build the Criterion test suite"          OFF)
option(ALGLANG_BUILD_TOOLS      "Build developer tools (disassembler...)" OFF)
option(ALGLANG_BUILD_GRAPHICS   "Build the raylib graphics module"        OFF)
option(ALGLANG_BUILD_NETWORK    "Build the network stdlib module"         OFF)
option(ALGLANG_ENABLE_ASAN      "Enable AddressSanitizer"                 OFF)
option(ALGLANG_ENABLE_UBSAN     "Enable UndefinedBehaviorSanitizer"       OFF)
option(ALGLANG_ENABLE_TSAN      "Enable ThreadSanitizer"                  OFF)
option(ALGLANG_ENABLE_COVERAGE  "Enable code coverage instrumentation"    OFF)
option(ALGLANG_WARNINGS_AS_ERRORS "Treat all compiler warnings as errors" OFF)
option(ALGLANG_USE_LTO          "Enable Link-Time Optimization"           OFF)

if(ALGLANG_USE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _ipo_supported OUTPUT _ipo_output)
    if(_ipo_supported)
        message(STATUS "[alglang] LTO enabled")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "[alglang] LTO not supported: ${_ipo_output}")
    endif()
endif()

add_library(alglang_compile_options INTERFACE)

target_compile_options(alglang_compile_options INTERFACE
    $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:
        # Strict warnings
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Wmissing-declarations
        -Wdouble-promotion
        -Wformat=2
        -Wformat-truncation
        -Wnull-dereference
        -Wundef
        -Wunused
        -Wconversion
        -Wsign-conversion
        -Wcast-align
        -Wcast-qual
        -Wold-style-definition
        -Wmissing-field-initializers
        -Wredundant-decls
        -Wwrite-strings

        # Build-type specific
        $<$<CONFIG:Debug>:        -g3 -O0 -DALGLANG_DEBUG>
        $<$<CONFIG:Release>:      -O3 -DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
        $<$<CONFIG:MinSizeRel>:   -Os -DNDEBUG>
    >
    $<$<C_COMPILER_ID:MSVC>:
        /W4
        /WX
        /permissive-
        /w14242 /w14254 /w14263 /w14265 /w14287
        /we4289 /w14296 /w14311 /w14545 /w14546
        /w14547 /w14549 /w14555 /w14619 /w14640
        /w14826 /w14905 /w14906 /w14928
        $<$<CONFIG:Debug>:        /Od /Zi /DALGLANG_DEBUG>
        $<$<CONFIG:Release>:      /O2 /DNDEBUG>
    >
)

if(ALGLANG_WARNINGS_AS_ERRORS)
    target_compile_options(alglang_compile_options INTERFACE
        $<$<C_COMPILER_ID:GNU,Clang,AppleClang>: -Werror>
        $<$<C_COMPILER_ID:MSVC>: /WX>
    )
endif()

# target_compile_definitions(alglang_compile_options INTERFACE
#     ALGLANG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
#     ALGLANG_VERSION_MINOR=${PROJECT_VERSION_MINOR}
#     ALGLANG_VERSION_PATCH=${PROJECT_VERSION_PATCH}
#     $<$<BOOL:${ALGLANG_BUILD_GRAPHICS}>: ALGLANG_GRAPHICS_ENABLED>
#     $<$<BOOL:${ALGLANG_BUILD_NETWORK}>:  ALGLANG_NETWORK_ENABLED>
#     # POSIX feature macros (needed for file/net APIs on Linux/macOS)
#     $<$<NOT:$<C_COMPILER_ID:MSVC>>: _POSIX_C_SOURCE=200809L _XOPEN_SOURCE=700>
# )

# # subdirectories
# add_subdirectory(src/i18n)
# add_subdirectory(src/diagnostics)
# add_subdirectory(src/runtime)
# add_subdirectory(src/core)
# add_subdirectory(src/builtins)

# if(ALGLANG_BUILD_GRAPHICS)
#     add_subdirectory(src/graphics)
# endif()

# # core
# add_library(alglang_core STATIC)

# target_link_libraries(alglang_core
#     PUBLIC
#         alglang_compile_options
#     PRIVATE
#         alglang_i18n
#         alglang_diagnostics
#         alglang_runtime
#         alglang_pipeline
#         alglang_builtins
#         $<$<BOOL:${ALGLANG_BUILD_GRAPHICS}>: alglang_graphics>
#         $<$<BOOL:${ALGLANG_BUILD_NETWORK}>:  ${ALGLANG_NET_LIBS}>
#         m
# )

# target_include_directories(alglang_core
#     PUBLIC
#         "${CMAKE_CURRENT_SOURCE_DIR}/include"
#         "${CMAKE_CURRENT_BINARY_DIR}/include"
#     PRIVATE
#         "${CMAKE_CURRENT_SOURCE_DIR}/src"
# )

# # Apply sanitizers and coverage to the core library
# alglang_apply_sanitizers(alglang_core)
# alglang_apply_coverage(alglang_core)

# Main executable
# add_executable(alglang src/main.c)
add_executable(${PROJECT_NAME} src/main.c)

# target_link_libraries(${PROJECT_NAME}
#     PRIVATE
#         alglang_core
#         alglang_compile_options
# )

# target_include_directories(alglang
#     PRIVATE
#         "${CMAKE_CURRENT_SOURCE_DIR}/src"
#         "${CMAKE_CURRENT_SOURCE_DIR}/include"
#         "${CMAKE_CURRENT_BINARY_DIR}/include"
# )

# alglang_apply_sanitizers(alglang)

# # Strip Release binary on Linux/macOS
# if(CMAKE_BUILD_TYPE STREQUAL "Release")
#     if(UNIX)
#         add_custom_command(TARGET alglang POST_BUILD
#             COMMAND ${CMAKE_STRIP} $<TARGET_FILE:alglang>
#             COMMENT "Stripping release binary"
#         )
#     endif()
# endif()

# # tools
# if(ALGLANG_BUILD_TOOLS)
#     add_subdirectory(tools/alglang-dis)
#     add_subdirectory(tools/gen_lang_template)
# endif()

# # tests
# if(ALGLANG_BUILD_TESTS)
#     enable_testing()
#     add_subdirectory(tests)
# endif()

# include(GNUInstallDirs)

# install(TARGETS alglang
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# install(TARGETS alglang_core
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )

# install(DIRECTORY include/alglang
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )

# install(DIRECTORY languages/
#     DESTINATION ${CMAKE_INSTALL_DATADIR}/alglang/languages
# )

# install(DIRECTORY stdlib/
#     DESTINATION ${CMAKE_INSTALL_DATADIR}/alglang/stdlib
# )

