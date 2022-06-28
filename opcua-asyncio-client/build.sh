#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

# "Debug" or empty for release build
BUILD_TYPE="$1"
if [ -z "$BUILD_TYPE" ]; then
  BUILD_TYPE="Release"
fi
BUILD_DIR="${SCRIPT_DIR}/build"
SRC_DIR="${SCRIPT_DIR}/src"
TOOLCHAIN_PYTHON="$(which python3)"
TOOLCHAIN_PIP="${TOOLCHAIN_PYTHON} -m pip"

if [[ ! -d ${BUILD_DIR} ]]; then
  mkdir -p ${BUILD_DIR}
fi

/bin/bash -c "set -o pipefail \
  && cd ${BUILD_DIR} \
  && ${TOOLCHAIN_PIP} install pyinstaller \
  && ${TOOLCHAIN_PIP} install asyncua cryptography \
  && pyinstaller -F ${SRC_DIR}/main.py --distpath ${BUILD_DIR}/dist --name opcua-asyncio-client --clean"

EXIT_CODE=$?
if [[ ${EXIT_CODE} -eq 0 ]]; then
  echo "Build was successful"
else
  echo "Build failed with ERROR: ${EXIT_CODE}"
  exit ${EXIT_CODE}
fi
