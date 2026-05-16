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
