#pragma once

#include <cstdarg>    // std::va_list
#include <exception>  // std::exception
#include <string>     // std::string

#include "Level.h"

namespace ggolbik {
namespace opcua {
namespace logging {

/**
 * @brief Interface Logger. It is expected that logging takes place through
 * concrete implementations of this interface.
 */
class ILogger {
 public:
  /**
   * @brief Returns the current log level.
   */
  virtual Level getLevel() const = 0;
  /**
   * @brief Sets the log level.
   * 
   * @param level The log level.
   */
  virtual void setLevel(Level level) = 0; 
  /**
   * @brief Checks whether the logger instance is enabled for the given log
   * level?
   */
  virtual bool isLevelEnabled(Level level) const = 0;
  /**
   * @brief Log an exception at the specified log level.
   *
   * @param tag The source of the log message.
   * @param level The log level.
   * @param e The thrown exception.
   */
  virtual void log(const std::string& tag, Level level, const std::string& category, const std::exception& e) = 0;
  /**
   * @brief Log a message at the specified log level.
   *
   * @param tag The source of the log message.
   * @param level The log level.
   * @param format The message format.
   * @param ... The message arguments.
   */
  virtual void log(const std::string& tag, Level level, const std::string& category, const std::string& format,
                   ...) = 0;
  /**
   * @brief Log an exception at the specified log level with an accompanying
   * message.
   *
   * @param tag The source of the log message.
   * @param level The log level.
   * @param e The thrown exception.
   * @param format The message format.
   * @param ... The message arguments.
   */
  virtual void log(const std::string& tag, Level level, const std::string& category, const std::exception& e,
                   const std::string& format, ...) = 0;
  /**
   * @brief Log a message at the specified log level.
   *
   * @param tag The source of the log message.
   * @param level The log level.
   * @param format The message format.
   * @param args The message arguments.
   */
  virtual void vlog(const std::string& tag, Level level, const std::string& category, const std::string& format,
                    std::va_list args) = 0;
  /**
   * @brief Log an exception at the specified log level with an accompanying
   * message.
   *
   * @param tag The source of the log message.
   * @param level The log level.
   * @param e The thrown exception.
   * @param format The message format.
   * @param args The message arguments.
   */
  virtual void vlog(const std::string& tag, Level level, const std::string& category, const std::exception& e,
                    const std::string& format, std::va_list args) = 0;
};

}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik