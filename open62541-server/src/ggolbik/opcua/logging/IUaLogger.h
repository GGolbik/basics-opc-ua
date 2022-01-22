#pragma once

#include <open62541/plugin/log.h>

#include "ILogger.h"

namespace ggolbik {
namespace opcua {
namespace logging {

class IUaLogger : public ILogger {
 public:
  /**
   * @brief Transforms logger to UA_Logger
   *
   * @return UA_Logger
   */
  virtual UA_Logger toUaLogger() = 0;
  /**
   * @brief Transforms Level to UA_LogLevel
   */
  virtual UA_LogLevel toUaLogLevel(Level level) = 0;
  /**
   * @brief Transforms UA_LogLevel to Level
   */
  virtual Level toLevel(UA_LogLevel level) = 0;
  /**
   * @brief Transforms UA_LogCategory to Category
   */
  virtual std::string toString(UA_LogCategory category) = 0;
};

}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik