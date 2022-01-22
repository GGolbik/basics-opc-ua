#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

LIBS_DIR="${SCRIPT_DIR}"
BUILD_DIR="${SCRIPT_DIR}/../build/open62541-windows-x86_64"
OPEN62541_DIR="${LIBS_DIR}/open62541"
TOOLCHAIN_MAKE=$(which make)
TOOLCHAIN_CMAKE=$(which cmake)
TOOLCHAIN_FILE=${SCRIPT_DIR}/../cmake/windows-x86_64.cmake
DEST_DIR="${BUILD_DIR}/target"

if [[ ! -d ${BUILD_DIR} ]]; then
  mkdir -p ${BUILD_DIR}
  # Build options: https://open62541.org/doc/current/building.html#main-build-options
  /bin/bash -c "set -o pipefail \
    && cd ${BUILD_DIR} \
    && ${TOOLCHAIN_CMAKE} \
    -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} \
    ${OPEN62541_DIR} \
    && ${TOOLCHAIN_CMAKE} --build . \
    && ${TOOLCHAIN_MAKE} install DESTDIR=${DEST_DIR} \
    && ${TOOLCHAIN_CMAKE} --build . --target clean \
    && ${TOOLCHAIN_MAKE} clean"
else
  echo "${BUILD_DIR} already exists."
fi
