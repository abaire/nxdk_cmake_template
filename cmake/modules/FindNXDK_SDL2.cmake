if (NOT TARGET NXDK::SDL2)
    if (NOT DEFINED NXDK_DIR)
        message(FATAL_ERROR "NXDK_DIR variable must be set")
    endif ()

    add_library(nxdk_sdl2 STATIC IMPORTED)
    set_target_properties(
            nxdk_sdl2
            PROPERTIES
            IMPORTED_LOCATION "${NXDK_DIR}/lib/libSDL2.lib"
    )

    add_library(NXDK::SDL2 INTERFACE IMPORTED)
    target_link_libraries(
            NXDK::SDL2
            INTERFACE
            nxdk_sdl2
    )
    target_include_directories(
            NXDK::SDL2
            SYSTEM INTERFACE
            "${NXDK_DIR}/lib/sdl/SDL2/include"
    )
    target_compile_definitions(
            NXDK::SDL2
            INTERFACE
            XBOX
    )
endif ()
