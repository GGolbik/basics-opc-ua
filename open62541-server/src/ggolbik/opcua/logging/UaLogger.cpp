#include "UaLogger.h"

namespace ggolbik {
namespace opcua {
namespace logging {

UaLogger::UaLogger(const std::string& tag, std::shared_ptr<ILogger> impl)
    : tag(tag), implPtr(impl) {
  // empty
}

void UaLogger::vlog(void* logContext, UA_LogLevel level,
                    UA_LogCategory category, const char* msg,
                    std::va_list args) {
  if (logContext != nullptr) {
    static_cast<UaLogger*>(logContext)->vlog(level, category, msg, args);
  }
}

void UaLogger::clear(void*) {
  // not implemented yet
}

Level UaLogger::getLevel() const {
  if (!this->implPtr) {
    return Level::off;
  }
  return this->implPtr->getLevel();
}

bool UaLogger::isLevelEnabled(Level level) const {
  if (!this->implPtr) {
    return false;
  }
  return this->implPtr->isLevelEnabled(level);
}

void UaLogger::setLevel(Level level) {
  if (this->implPtr) {
    this->implPtr->setLevel(level);
  }
}

void UaLogger::log(const std::string& tag, Level level,
                   const std::string& category, const std::exception& e) {
  if (this->implPtr) {
    this->implPtr->log(tag, level, category, e);
  }
}

void UaLogger::log(const std::string& tag, Level level,
                   const std::string& category, const std::string& format,
                   ...) {
  if (this->implPtr) {
    va_list args;
    // Name of the last named parameter in the function definition: format
    va_start(args, format);
    this->vlog(tag, level, category, format, args);
    va_end(args);
  }
}

void UaLogger::log(const std::string& tag, Level level,
                   const std::string& category, const std::exception& e,
                   const std::string& format, ...) {
  if (this->implPtr) {
    va_list args;
    // Name of the last named parameter in the function definition: format
    va_start(args, format);
    this->vlog(tag, level, category, e, format, args);
    va_end(args);
  }
}

void UaLogger::vlog(const std::string& tag, Level level,
                    const std::string& category, const std::string& format,
                    std::va_list args) {
  if (this->implPtr) {
    this->implPtr->vlog(tag, level, category, format, args);
  }
}

void UaLogger::vlog(const std::string& tag, Level level,
                    const std::string& category, const std::exception& e,
                    const std::string& format, std::va_list args) {
  if (this->implPtr) {
    this->implPtr->vlog(tag, level, category, e, format, args);
  }
}

UA_Logger UaLogger::toUaLogger() {
  return {
      &UaLogger::vlog,           // log
      static_cast<void*>(this),  // this instance
      &UaLogger::clear           // clear
  };
}

UA_LogLevel UaLogger::toUaLogLevel(Level level) {
  switch (level) {
    case Level::trace:
      return UA_LOGLEVEL_TRACE;
    case Level::debug:
      return UA_LOGLEVEL_DEBUG;
    case Level::info:
      return UA_LOGLEVEL_INFO;
    case Level::warn:
      return UA_LOGLEVEL_WARNING;
    case Level::error:
      return UA_LOGLEVEL_ERROR;
    case Level::fatal:
      return UA_LOGLEVEL_FATAL;
    default:
      return UA_LOGLEVEL_TRACE;
  }
}

Level UaLogger::toLevel(UA_LogLevel level) {
  switch (level) {
    case UA_LOGLEVEL_TRACE:
      return Level::trace;
    case UA_LOGLEVEL_DEBUG:
      return Level::debug;
    case UA_LOGLEVEL_INFO:
      return Level::info;
    case UA_LOGLEVEL_WARNING:
      return Level::warn;
    case UA_LOGLEVEL_ERROR:
      return Level::error;
    case UA_LOGLEVEL_FATAL:
      return Level::fatal;
    default:
      return Level::off;
  }
}

std::string UaLogger::toString(UA_LogCategory category) {
  switch (category) {
    case UA_LOGCATEGORY_NETWORK:
      return "network";
    case UA_LOGCATEGORY_SECURECHANNEL:
      return "securechannel";
    case UA_LOGCATEGORY_SESSION:
      return "session";
    case UA_LOGCATEGORY_SERVER:
      return "server";
    case UA_LOGCATEGORY_CLIENT:
      return "client";
    case UA_LOGCATEGORY_SECURITYPOLICY:
      return "securitypolicy";
    case UA_LOGCATEGORY_USERLAND:
      return "userland";
    default:
      return "";
  }
}

void UaLogger::vlog(UA_LogLevel level, UA_LogCategory category, const char* msg,
                    std::va_list args) {
  const std::string message = std::string(msg);
  const std::string categoryStr = this->toString(category);

  this->implPtr->vlog(this->tag, this->toLevel(level), categoryStr, message,
                      args);
}

}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik