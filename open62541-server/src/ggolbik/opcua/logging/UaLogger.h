#pragma once

#include <open62541/plugin/log.h>

#include <cstdarg>  // std::va_list
#include <memory>   // std::shared_ptr
#include <string>   // std::string

#include "ILogger.h"
#include "IUaLogger.h"

namespace ggolbik {
namespace opcua {
namespace logging {

/**
 * @brief Logger implementation which transforms UA log to the ILogger
 * interface.
 */
class UaLogger : public IUaLogger {
 public:  // constructor/destructor
  explicit UaLogger(const std::string& tag, std::shared_ptr<ILogger> impl);

 public:  // static methods
  /**
   * @brief Log a message. The message string and following varargs are
   * formatted according to the rules of the printf command. Use the convenience
   * macros below that take the minimum log level defined in ua_config.h into
   * account.
   *
   * @param logContext The UaLogger instance.
   * @param level The log level of the message.
   * @param category The log category of the message.
   * @param msg The format string.
   * @param args The message arguments.
   */
  static void vlog(void* logContext, UA_LogLevel level, UA_LogCategory category,
                   const char* msg, std::va_list args);
  /**
   * @brief  Clean up the logger plugin
   *
   * @param logContext The UaLogger instance.
   */
  static void clear(void* logContext);

 public:  // IUaLogger methods
  UA_Logger toUaLogger() override final;
  UA_LogLevel toUaLogLevel(Level level) override final;
  Level toLevel(UA_LogLevel level) override final;
  std::string toString(UA_LogCategory category) override final;

 public:  // ILogger methods
  Level getLevel() const override;
  void setLevel(Level level) override;
  bool isLevelEnabled(Level level) const override;
  void log(const std::string& tag, Level level, const std::string& category,
           const std::exception& e) override;
  void log(const std::string& tag, Level level, const std::string& category,
           const std::string& format, ...) override;
  void log(const std::string& tag, Level level, const std::string& category,
           const std::exception& e, const std::string& format, ...) override;
  void vlog(const std::string& tag, Level level, const std::string& category,
            const std::string& format, std::va_list args) override;
  void vlog(const std::string& tag, Level level, const std::string& category,
            const std::exception& e, const std::string& format,
            std::va_list args) override;

 private:
  /**
   * @brief Transforms UA log to ILogger log
   *
   * @param level
   * @param category
   * @param msg
   * @param args
   */
  void vlog(UA_LogLevel level, UA_LogCategory category, const char* msg,
            std::va_list args);

 private:
  const std::string tag;
  std::shared_ptr<ILogger> implPtr;
};

}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik