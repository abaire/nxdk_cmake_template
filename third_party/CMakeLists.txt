include(ExternalProject)

find_program(MAKE_COMMAND NAMES gmake make REQUIRED)

set(NXDK_DEBUG_FLAG "n")
set(NXDK_LTO_FLAG "n")
ExternalProject_Add(
        nxdk_project
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nxdk
        BUILD_IN_SOURCE ON
        CONFIGURE_COMMAND ""
        BUILD_COMMAND
        bash -c "
                  pushd bin &&
                  . activate -s &&
                  popd &&
                  DEBUG=${NXDK_DEBUG_FLAG} LTO=${NXDK_LTO_FLAG} NXDK_ONLY=y ${MAKE_COMMAND} -j &&
                  ${MAKE_COMMAND} cxbe -j &&
                  ${MAKE_COMMAND} extract-xiso -j
              "
        LOG_BUILD ON
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS

)
ExternalProject_Get_Property(nxdk_project source_dir)
set(NXDK_DIR ${source_dir})


# Full featured printf
set(PRINTF_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/third_party/printf-src")
set(PRINTF_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/third_party/printf-build")
set(PRINTF_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/third_party/printf-install")
set(PRINTF_ARTIFACT "${PRINTF_INSTALL_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}printf${CMAKE_STATIC_LIBRARY_SUFFIX}")
ExternalProject_Add(
        printf_project
        GIT_REPOSITORY "https://github.com/mpaland/printf.git"
        GIT_TAG "master"
        GIT_SHALLOW TRUE

        SOURCE_DIR "${PRINTF_SOURCE_DIR}"
        BINARY_DIR "${PRINTF_BINARY_DIR}"
        INSTALL_DIR "${PRINTF_INSTALL_DIR}"

        CONFIGURE_COMMAND ""

        BUILD_COMMAND
        "${CMAKE_C_COMPILER}"
        -O3
        -Wno-everything
        -c "${PRINTF_SOURCE_DIR}/printf.c"
        -o "${PRINTF_BINARY_DIR}/printf.o"
        COMMAND
        "${CMAKE_AR}"
        rc
        "${PRINTF_INSTALL_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}printf${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "${PRINTF_BINARY_DIR}/printf.o"

        INSTALL_COMMAND ""

        BUILD_BYPRODUCTS
        "${PRINTF_ARTIFACT}"

        LOG_BUILD ON
)

add_library(printf STATIC IMPORTED GLOBAL)

target_include_directories(
        printf
        INTERFACE
        "${PRINTF_SOURCE_DIR}"
)

set_property(
        TARGET printf
        PROPERTY IMPORTED_LOCATION
        "${PRINTF_ARTIFACT}"
)

add_dependencies(
        printf
        printf_project
)

add_library(Printf::printf ALIAS printf)
