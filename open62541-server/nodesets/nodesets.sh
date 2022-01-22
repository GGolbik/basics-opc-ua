#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

LIBS_DIR="${SCRIPT_DIR}/../libs"
OPEN62541_DIR="${LIBS_DIR}/open62541"
COMPILER_DIR="${OPEN62541_DIR}/tools/nodeset_compiler"
NODESETS_DIR="${OPEN62541_DIR}/deps/ua-nodeset"
SRC_DIR="${SCRIPT_DIR}/../src"
GENERATED_DIR="${SRC_DIR}/nodesets"

mkdir -p ${GENERATED_DIR}

python3 ${COMPILER_DIR}/nodeset_compiler.py \
  --types-array=UA_TYPES \
  --existing ${NODESETS_DIR}/Schema/Opc.Ua.NodeSet2.xml \
  --xml ${NODESETS_DIR}/DI/Opc.Ua.Di.NodeSet2.xml \
  --bsd ${NODESETS_DIR}/DI/Opc.Ua.Di.Types.bsd \
  ${GENERATED_DIR}/di

  python3 ${COMPILER_DIR}/nodeset_compiler.py \
  --types-array=UA_TYPES \
  --existing ${NODESETS_DIR}/Schema/Opc.Ua.NodeSet2.xml \
  --xml ${SCRIPT_DIR}/example_nodeset.xml \
  ${GENERATED_DIR}/example_nodeset