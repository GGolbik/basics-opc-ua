# name of the target platform
set(CMAKE_SYSTEM_NAME Linux)
# (and optionally target processor architecture):
set(CMAKE_SYSTEM_PROCESSOR armhf)

# path to the toolchain binaries (C compiler, C++ compiler, linker, etc.):
#set(CMAKE_AR <path_to_ar>)
#set(CMAKE_ASM_COMPILER <path_to_assembler>)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
#set(CMAKE_LINKER <path_to_linker>)
#set(CMAKE_OBJCOPY <path_to_objcopy>)
#set(CMAKE_RANLIB <path_to_ranlib>)
#set(CMAKE_SIZE <path_to_size>)
#set(CMAKE_STRIP <path_to_strip>)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

# required compilation and linking flags on that particular platform:
#set(CMAKE_C_FLAGS <c_flags>)
#set(CMAKE_CXX_FLAGS <cpp_flags>)
#set(CMAKE_C_FLAGS_DEBUG <c_flags_for_debug>)
#set(CMAKE_C_FLAGS_RELEASE <c_flags_for_release>)
#set(CMAKE_CXX_FLAGS_DEBUG <cpp_flags_for_debug>)
#set(CMAKE_CXX_FLAGS_RELEASE <cpp_flags_for_release>)
#set(CMAKE_EXE_LINKER_FLAGS <linker_flags>)

set(OPENSSL_ROOT_DIR "${PROJECT_SOURCE_DIR}/build/openssl-linux-armhf/target/usr/local")

### open62541
### Build options doc: https://open62541.org/doc/current/building.html#main-build-options
### 
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/build/open62541-linux-armhf/target/usr/local/lib/cmake")
# set library type: default is static
#option(BUILD_SHARED_LIBS "Enable building open62541 as shared libraries (dll/so)" ON)
# set log level: default is Info
set(UA_LOGLEVEL 300 CACHE STRING "Level at which logs shall be reported by the open62541 SDK" FORCE) # Info
# Use FULL opc ua core namespace and not reduced
set(UA_NAMESPACE_ZERO "FULL" CACHE STRING "Completeness of the generated namespace zero (minimal/reduced/full)" FORCE)
# Further options shown with default values.
#set(UA_MULTITHREADING 0 CACHE STRING "Level of multithreading (0-99: No Multithreading, >=100: Thread-Safe API")
#option(UA_ENABLE_NODEMANAGEMENT "Enable dynamic addition and removal of nodes at runtime" ON)
#option(UA_ENABLE_DIAGNOSTICS "Enable diagnostics information exposed by the server" ON)
#option(UA_ENABLE_METHODCALLS "Enable the Method service set" ON)
#option(UA_ENABLE_SUBSCRIPTIONS "Enable subscriptions support" ON)
#option(UA_ENABLE_SUBSCRIPTIONS_EVENTS "Enable event monitoring" ON)
#option(UA_ENABLE_SUBSCRIPTIONS_ALARMS_CONDITIONS "Enable the use of A&C. (EXPERIMENTAL)" OFF)
#option(UA_ENABLE_DA "Enable OPC UA DataAccess (Part 8) definitions" ON)
#option(UA_ENABLE_HISTORIZING "Enable basic support for historical access (client and server)" OFF)
#option(UA_ENABLE_DISCOVERY "Enable Discovery Service (LDS)" OFF)
#option(UA_ENABLE_DISCOVERY_MULTICAST "Enable Discovery Service with multicast support (LDS-ME)" OFF)
#option(UA_ENABLE_DISCOVERY_SEMAPHORE "Enable Discovery Semaphore support" ON)
#option(UA_ENABLE_TYPEDESCRIPTION "Add the type and member names to the UA_DataType structure" ON)
#option(UA_ENABLE_STATUSCODE_DESCRIPTIONS "Enable conversion of StatusCode to human-readable error message" ON)
#option(UA_DEBUG_DUMP_PKGS "Dump every package received by the server as hexdump format" OFF)
#option(UA_ENABLE_PARSING "Enable parsing human readable formats of builtin data types (Guid, NodeId, etc.). Utility functions that require parsing (e.g. NodeId expressions)." ON)
#option(UA_ENABLE_WEBSOCKET_SERVER "Enable websocket support (uses libwebsockets)" OFF)