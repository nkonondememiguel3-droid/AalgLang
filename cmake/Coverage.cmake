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

