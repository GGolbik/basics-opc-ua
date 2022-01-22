#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

# "Debug" or empty for release build
BUILD_TYPE=$1
BUILD_DIR="${SCRIPT_DIR}/build/default"
TOOLCHAIN_CMAKE=$(which cmake)
BUILD_SCRIPT="${SCRIPT_DIR}/scripts/build-linux-amd64.sh"
BUILD_SRC_DIR="${SCRIPT_DIR}/build/linux-amd64"

/bin/bash -c "set -o pipefail \
  && bash ${BUILD_SCRIPT} ${BUILD_TYPE} \
  && ln -s -f ${BUILD_SRC_DIR} ${BUILD_DIR}"

EXIT_CODE=$?
if [[ ${EXIT_CODE} -eq 0 ]]; then
  echo "Build was successful"
else
  echo "Build failed with ERROR: ${EXIT_CODE}"
  exit ${EXIT_CODE}
fi
