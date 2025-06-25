cmake_minimum_required(VERSION 3.30)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
include(PrebuildNXDK)

project(nxdk_cmake_template)

set(
        CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/"
)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Generate verbose Makefile")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(_CMAKE_PROCESSING_LANGUAGE "CXX")

# Determine if this build is for the Xbox or the host system.
if (CMAKE_TOOLCHAIN_FILE MATCHES "toolchain-nxdk.cmake")
    set(IS_TARGET_BUILD ON)
else ()
    message (FATAL_ERROR "This project must be built with the nxdk toolchain (`-DCMAKE_TOOLCHAIN_FILE=<YOUR_NXDK_DIR>/share/toolchain-nxdk.cmake`)")
endif ()

add_subdirectory(src)
