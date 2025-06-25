set(NXDK_SOURCE_DIR "${CMAKE_SOURCE_DIR}/third_party/nxdk")
set(NXDK_BUILD_MARKER "${NXDK_SOURCE_DIR}/lib/libnxdk.lib")

if (NOT EXISTS "${NXDK_BUILD_MARKER}")
    message(STATUS "Bootstrapping NXDK build in ${NXDK_SOURCE_DIR}.")

    find_program(MAKE_COMMAND NAMES gmake make REQUIRED)

    set(NXDK_DEBUG_FLAG "n" CACHE STRING "Enable debug build for the NXDK submodule. 'y' or 'n'.")
    set(NXDK_LTO_FLAG "n" CACHE STRING "Enable link time optimization for the NXDK submodule. 'y' or 'n'.")

    set(
            NXDK_BUILD_COMMAND
            "
                export NXDK_DIR=\"${NXDK_SOURCE_DIR}\"
                cd bin &&
                . activate -s &&
                cd .. &&
                DEBUG=${NXDK_DEBUG_FLAG} LTO=${NXDK_LTO_FLAG} NXDK_ONLY=y ${MAKE_COMMAND} -j &&
                ${MAKE_COMMAND} cxbe -j &&
                ${MAKE_COMMAND} extract-xiso -j
            "
    )

    execute_process(
            COMMAND /bin/bash -c "${NXDK_BUILD_COMMAND}"
            WORKING_DIRECTORY "${NXDK_SOURCE_DIR}"
            RESULT_VARIABLE nxdk_build_result
            OUTPUT_VARIABLE nxdk_build_output
            ERROR_VARIABLE nxdk_build_output
    )

    if (NOT nxdk_build_result EQUAL 0)
        message(FATAL_ERROR "Failed to build the nxdk submodule. Error code: ${nxdk_build_result}\nOutput:\n${nxdk_build_output}")
    else ()
        message(STATUS "NXDK bootstrap build completed successfully.")
    endif ()

endif ()
