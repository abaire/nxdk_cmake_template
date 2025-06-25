nxdk_cmake_template
====

Basic sample project illustrating use of CMake with the [XboxDev/nxdk](https://github.com/XboxDev/nxdk).

## Prerequisites

The sample project uses https://pypi.org/project/nv2a-vsh to build a trivial passthrough vertex shader. It may be
installed by running `pip install -r requirements.txt` from this directory.

## CLion

### Building

Under Settings > Build, Execution, Deployment > CMake

- Provide the path to the nxdk toolchain file under CMake Options

  `-DCMAKE_TOOLCHAIN_FILE=$CMakeProjectDir$/third_party/nxdk/share/toolchain-nxdk.cmake`

- Optionally modify the `PATH` variable to make sure the path to your chosen version of Clang comes first.

### Debugging

#### Using [xemu](xemu.app)

1. Create a new `Embedded GDB Server` target
1. Set the Target to the `<your project name>_xiso` target
1. Set the Executable to the `<your project name>` binary (it should be the only thing in the dropdown)
1. Set `Upload executable` to `Never`
1. Set `'target remote' args` to `127.0.0.1:1234`
1. Set `GDB Server` to the path to the xemu binary
1. Set `GDB Server args` to `-s -S -dvd_path "$CMakeCurrentBuildDir$/xiso/<your_project_name>_xiso/<your_project_name>_xiso.iso"` (the `-S` is
   optional and will cause xemu to wait for the debugger to connnect)
1. Under `Advanced GDB Server Options`
1. Set "Working directory" to `$ProjectFileDir$`
1. On macOS, set "Environment variables"
   to
   `DYLD_FALLBACK_LIBRARY_PATH=/<the full path to your xemu.app bundle>/Contents/Libraries/<the architecture for your platform, e.g., arm64>`
1. Set "Reset command" to `Never`

#### Using [xbdm_gdb_bridge](https://github.com/abaire/xbdm_gdb_bridge) and a devkit/XBDM-enabled Xbox

1. Create a new `Embedded GDB Server` target
1. Set the Target to the `<your project name>_xiso` target
1. Set the Executable to the `<your project name>` binary (it should be the only thing in the dropdown)
1. Set `Upload executable` to `Never`
1. Set `'target remote' args` to `127.0.0.1:1999`
1. Set `GDB Server` to the path to the xbdm_gdb_bridge binary (e.g., `<some_path>/xbdm_gdb_bridge`)
1. Set `GDB Server args` to `<your_xbox_ip> -v3 -s -- gdb :1999 e:\$CMakeCurrentTargetName$`
1. Under `Advanced GDB Server Options`, set `Reset command` to `Never`

To perform automatic deployment after builds:

1. Under the `Before launch` section, click the `+` button and add a new `Run external tool` entry.
1. Set the `Description` field to something like `Sync program and resources to the XBOX`
1. Set the `Program` field to the path to the xbdm_gdb_bridge binary (same as above)
1. Set `Arguments` to
   `<your_xbox_ip> -- mkdir e:\$CMakeCurrentTargetName$ && %syncdir $CMakeCurrentBuildDir$/xbe/xbe_file e:\$CMakeCurrentTargetName$ -f`
1. Set `Working directory` to `$ProjectFileDir$`
