# TODO: Set the title of the XBE. This is displayed in the debug dashboard.
set(XBE_TITLE "NXDK CMake Template")

include(XBEUtils REQUIRED)

find_package(NXDK REQUIRED)

# TODO: Tailor these to the needs of your application
find_package(NXDK_SDL2 REQUIRED)
find_package(NXDK_SDL2_Image REQUIRED)
find_package(NXDK_SDL2_Test REQUIRED)
find_package(NXDK_SDL_TTF REQUIRED)
find_package(Threads REQUIRED)

configure_file(configure.h.in configure.h)

macro(set_opt_compile_and_link_options TARGET_NAME)
    target_compile_options(
            "${TARGET_NAME}"
            PRIVATE
            -O3
            -fno-strict-aliasing
            -Wall
            $<$<COMPILE_LANGUAGE:CXX>:-Wno-builtin-macro-redefined>   # Suppress warning from NXDK undef of __STDC_NO_THREADS__
            -D_USE_MATH_DEFINES
    )
    target_link_options(
            "${TARGET_NAME}"
            PRIVATE
            "/debug:none"
    )
endmacro()

macro(set_compile_and_link_options TARGET_NAME)
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_options(
                "${TARGET_NAME}"
                PRIVATE
                -g
                -gdwarf-4
                -O0
                -Wall
                -fstandalone-debug
                $<$<COMPILE_LANGUAGE:CXX>:-Wno-builtin-macro-redefined>   # Suppress warning from NXDK undef of __STDC_NO_THREADS__
                -D_USE_MATH_DEFINES
        )
        target_link_options(
                "${TARGET_NAME}"
                PRIVATE
                "/debug:full"
        )
    else ()
        set_opt_compile_and_link_options("${TARGET_NAME}")
    endif ()
endmacro()

# Vertex shaders compiled using https://pypi.org/project/nv2a-vsh/
include(NV2A_VSH REQUIRED)
generate_nv2a_vshinc_files(
        generated_nv2a_vertex_shaders
        SOURCES
        shaders/passthrough.vsh
)

add_executable(
        ${PROJECT_NAME}
        main.cpp
)

# Pull debug info out of the binary into a host-side linked binary.
split_debug(${PROJECT_NAME})

set(EXECUTABLE_BINARY "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe")
set_compile_and_link_options(${PROJECT_NAME})
target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        generated_nv2a_vertex_shaders
        NXDK::NXDK
        NXDK::NXDK_CXX
        NXDK::SDL2
        NXDK::SDL2_Image
        NXDK::SDL_TTF
)

add_xbe(
        xbe_file "${EXECUTABLE_BINARY}"
        TITLE "${XBE_TITLE}"
        RESOURCE_ROOTS
        "${CMAKE_SOURCE_DIR}/resources"
        RESOURCE_DIRS
        "${CMAKE_SOURCE_DIR}/resources"
)
add_xiso(${PROJECT_NAME}_xiso xbe_file)
