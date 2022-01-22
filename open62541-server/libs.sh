#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

LIBS_DIR="${SCRIPT_DIR}/libs"
OPENSSL_DIR="${LIBS_DIR}/openssl"
OPEN62541_DIR="${LIBS_DIR}/open62541"

if [[ ! -d ${OPENSSL_DIR} ]]; then
  # The official OpenSSL Git Repository is located at git.openssl.org.
  # There is a GitHub mirror of the repository at github.com/openssl/openssl, which is updated automatically from the former on every commit.
  # https://www.openssl.org/source/
  #git clone git://git.openssl.org/openssl.git -b openssl-3.0.0 ${OPENSSL_DIR}
  git clone git://git.openssl.org/openssl.git -b OpenSSL_1_1_1m ${OPENSSL_DIR}
fi

if [[ ! -d ${OPEN62541_DIR} ]]; then
  # open62541 is an open source C (C99) implementation of OPC UA licensed under the Mozilla Public License v2.0.
  # https://open62541.org/
  # https://github.com/open62541/open62541
  # git clone https://github.com/open62541/open62541.git -b 1.3 ${OPEN62541_DIR}
  git clone https://github.com/open62541/open62541.git ${OPEN62541_DIR}
  cd ${OPEN62541_DIR}
  git submodule update --init --recursive
  cd -
fi

# Prints available targets
#${OPENSSL_DIR}/Configure LIST
