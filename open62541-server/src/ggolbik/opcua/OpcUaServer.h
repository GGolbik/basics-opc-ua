#pragma once

#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <memory>

#include "logging/ILogger.h"
#include "logging/IUaLogger.h"

namespace ggolbik {
namespace opcua {

using namespace ggolbik::opcua::logging;

/**
 * A proper functor to delete/free UA_Server
 */
struct UaServerDeleterFunctor {
  void operator()(UA_Server *p) const {
    if (p != nullptr) {
      ::UA_Server_delete(p);
    }
  }
};

class OpcUaServer {
 public:  // typedefs/usings
  /**
   * Using custom deleter with unique_ptr
   */
  using UaServerPtr = std::unique_ptr<UA_Server, UaServerDeleterFunctor>;

 public:
  static const std::string LogTag;

 public:
  OpcUaServer() = default;
  OpcUaServer(const OpcUaServer &rhs) = delete;
  OpcUaServer &operator=(const OpcUaServer &rhs) = delete;
  ~OpcUaServer();

 public:
  bool configure(std::shared_ptr<ILogger> logger);
  bool start();
  void stop();

 private:
  static std::string toString(UA_String &str);

 private:
  bool createServer();
  bool addNamespaceDi();
  bool addNamespaceMy();
  bool addDefaultInstance();
  bool addDiInstance();
  bool addMyNsInstance();
  bool setManufacturer(UA_NodeId& nodeId);

 private:
  UA_Boolean running = false;
  std::shared_ptr<ILogger> loggerPtr;
  std::unique_ptr<IUaLogger> uaLoggerPtr;
  UaServerPtr uaServerPtr;
};

}  // namespace opcua
}  // namespace ggolbik