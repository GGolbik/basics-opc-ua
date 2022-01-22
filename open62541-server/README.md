# OPC UA - Server with open62541

# Table of Contents

* [Build Project](#build-project)
* [Install Project](#install-project)
* [Build and Install with Docker](#build-and-install-with-docker)
* [OpenSSL](#openssl)
  * [Linux](#linux)
  * [Windows](#windows)
* [open62541](#open62541)
  * [Nodesets](#nodesets)
* [Cross Toolchain](#cross-toolchain)

# Build Project

Execute the `build.sh` script on Linux. There is no support for Windows yet.
The `build.sh` script executes the `amd64` build script and creates a symbolic link from `build/default` to `build/linux-amd64`.

# Install Project

Execute the `install.sh` script on Linux. There is no support for Windows yet.

# Build and Install with Docker

Execute the `docker.sh` script on Linux. There is no support for Windows yet.

# OpenSSL

The the [wiki](https://wiki.openssl.org/index.php/Compilation_and_Installation) for more info about OpenSSL build configuration.

For local development you could install the `libssl-dev` package but this is not required for this project.
~~~
sudo apt install libssl-dev
~~~

## Linux

The `build.sh` script is based on the installed `libssl-dev` package.

The scripts in the `scripts` directory build the OpenSSL by source.
In the `libs` directory are OpenSSL build scripts for each target:
- `openssl-linux-amd64.sh`
- `openssl-linux-arm64.sh`
- `openssl-linux-armel.sh`
- `openssl-linux-armhf.sh`
- `openssl-windows-x86_64.sh`
- `openssl-windows-x86.sh`

> There is a bug in version `1.1.1d` and `1.1.1e` which prevents to use `make` with `DESTDIR`. Read the comments in the build scripts for a solution.

> The target directory changed in the windows builds from `/target/usr/local` to `Program Files` since version `1.1.1f`. Read the comments in the windows `*.cmake` files.

## Windows

You must copy the `libssl-1_1-x64.dll` and `libcrypto-1_1-x64.dll` from the `example-8/build/openssl-<target>/bin` directory to windows in the same directory of your application or the application won't start.

You can also build OpenSSL with `no-shared` option but then you have to link against `crypt32` for windows.

# open62541

[open62541](http://open62541.org) is an open source and free implementation of OPC UA (OPC Unified Architecture) written in the common subset of the C99 and C++98 languages.
- [Documentation](https://open62541.org/doc/current/toc.html)

## Nodesets

The `nodesets` directory contains an example nodeset.

Exceute the `nodesets/nodesets.sh` script to generate the C++ code for the example nodeset and the DI nodeset which is taken from the open62541 library directory.

# Cross Toolchain

In the `cmake` directory are CMake toolchain files for each target:
- `build-linux-amd64.sh`
- `build-linux-arm64.sh`
- `build-linux-armel.sh`
- `build-linux-armhf.sh`
- `build-windows-x86_64.sh`
- `build-windows-x86.sh`

In the `script` directory are build scripts for each target which will use the toolchain files.

The docker script executes all scripts in the directory.

`-DCMAKE_SYSTEM_NAME`: name of the platform, for which we are building (target platform), `Linux`, `Windows` or `Darwin` for macOS. By default, this value is the same as `CMAKE_HOST_SYSTEM_NAME`, which means that we are building for local platform (no cross-compilation).
If your target is an embedded system without OS set `CMAKE_SYSTEM_NAME` to `Generic`.
