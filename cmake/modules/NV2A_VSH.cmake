# Provides `generate_nv2a_vshinc_files`, a function to compile nv2a vertex
# shaders written in nv2a assembly into `.vshinc` source files that may be
# included and uploaded using pbkit.

include(CMakeParseArguments)

# pip3 install nv2a-vsh
# https://pypi.org/project/nv2a-vsh/
# https://github.com/abaire/nv2a_vsh_asm
set(NV2AVSH nv2avsh)

# Usage:
#    generate_nv2a_vshinc_files(
#            OUTPUT_VARIABLE _VERTEX_SHADER_FILES
#            INCLUDE_DIRECTORIES_VARIABLE _VERTEX_SHADER_INCLUDE_DIRS
#            GENERATION_TARGET_VARIABLE _VERTEX_SHADER_GEN_TARGET
#            SOURCES
#            path/to/vsh_file.vsh
#            ...
#    )
#
# Then:
#   Add `${_VERTEX_SHADER_FILES}` to your sources list for the target using the
#      vshinc files.
#   Add `${_VERTEX_SHADER_INCLUDE_DIRS} to the `target_include_directories` for
#      that same target.
#   Add an `add_dependencies` declaration tying the target to `${_VERTEX_SHADER_GEN_TARGET}`
function(generate_nv2a_vshinc_files)

    set(
            single_value_options
            OUTPUT_VARIABLE
            INCLUDE_DIRECTORIES_VARIABLE
            GENERATION_TARGET_VARIABLE
    )
    set(
            multi_value_options
            SOURCES
    )
    cmake_parse_arguments(
            PARSE_ARGV
            0
            "NV2A_VSH"
            ""
            "${single_value_options}"
            "${multi_value_options}"
    )

    foreach (var_name IN LISTS single_value_options)
        if (NOT NV2A_VSH_${var_name})
            message(FATAL_ERROR "generate_nv2a_vshinc_files() requires the ${var_name} argument.")
        endif ()
    endforeach ()

    set(generated_sources)
    set(generated_source_dirs)

    foreach (src ${NV2A_VSH_SOURCES})
        get_filename_component(abs_src "${src}" REALPATH)
        get_filename_component(src_dirname "${src}" DIRECTORY)
        get_filename_component(src_basename "${src}" NAME_WE)

        set(output "${CMAKE_CURRENT_BINARY_DIR}/${src_dirname}/${src_basename}.vshinc")

        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${src_dirname}")
        add_custom_command(
                OUTPUT "${output}"
                COMMAND "${NV2AVSH}" "${abs_src}" "${output}"
                DEPENDS "${abs_src}"
        )

        list(APPEND generated_sources "${output}")
        list(APPEND generated_source_dirs "${CMAKE_CURRENT_BINARY_DIR}/${src_dirname}")
    endforeach ()

    set(generation_target "${NV2A_VSH_OUTPUT_VARIABLE}_gen")
    add_custom_target(
            ${generation_target}
            DEPENDS ${generated_sources}
    )

    list(REMOVE_DUPLICATES generated_source_dirs)

    set(${NV2A_VSH_OUTPUT_VARIABLE} ${generated_sources} PARENT_SCOPE)
    set(${NV2A_VSH_INCLUDE_DIRECTORIES_VARIABLE} ${generated_source_dirs} PARENT_SCOPE)
    set(${NV2A_VSH_GENERATION_TARGET_VARIABLE} ${generation_target} PARENT_SCOPE)

endfunction()
