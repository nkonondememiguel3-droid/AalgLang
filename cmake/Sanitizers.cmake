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

