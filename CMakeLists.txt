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

# getline library
add_library(getline SHARED getline.c)
target_include_directories(getline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME}
    src/main.c
    src/algarena.h
    src/algtoken.h
    src/alglexer.c
    src/alglexer.h
    src/algtype.c
    src/algtype.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE getline)

