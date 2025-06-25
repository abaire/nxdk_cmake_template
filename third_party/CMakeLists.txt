include(ExternalProject)

set(NXDK_DEBUG_FLAG "n")
set(NXDK_LTO_FLAG "n")
ExternalProject_Add(
        nxdk_project
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nxdk
        BUILD_IN_SOURCE ON
        CONFIGURE_COMMAND ""
        BUILD_COMMAND
        sh -c "
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
