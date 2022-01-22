#include "RootLogger.h"

#include <iostream>

namespace ggolbik {
namespace opcua {
namespace logging {

RootLogger::RootLogger() : Logger(Level::info) {}

RootLogger::RootLogger(Level level) : Logger(level) {}

RootLogger::RootLogger(
    Level level, const std::map<std::string, std::shared_ptr<ILogger>>& impls)
    : Logger(level) {
  this->loggers = impls;
}

void RootLogger::log(const std::string& tag, Level level,
                     const std::string& category, const std::exception& e) {
  for (auto& entry : this->loggers) {
    if (entry.second) {
      entry.second->log(tag, level, category, e);
    }
  }
}
void RootLogger::vlog(const std::string& tag, Level level,
                      const std::string& category, const std::string& format,
                      std::va_list args) {

  for (auto& entry : this->loggers) {
    if (entry.second) {
      std::va_list argsCopy;
      ::va_copy(argsCopy, args);
      entry.second->vlog(tag, level, category, format, argsCopy);
      ::va_end(argsCopy);
    }
  }
}
void RootLogger::vlog(const std::string& tag, Level level,
                      const std::string& category, const std::exception& e,
                      const std::string& format, std::va_list args) {
  for (auto& entry : this->loggers) {
    if (entry.second) {
      std::va_list argsCopy;
      ::va_copy(argsCopy, args);
      entry.second->vlog(tag, level, category, e, format, argsCopy);
      ::va_end(argsCopy);
    }
  }
}

bool RootLogger::addLogger(const std::string& name,
                           std::shared_ptr<ILogger> impl) {
  if (!impl) return false;
  std::map<std::string, std::shared_ptr<ILogger>>::iterator it =
      this->loggers.find(name);
  if ((it != this->loggers.end()) && it->second) {
    return false;
  }
  this->loggers[name] = impl;
  impl->setLevel(this->level);
  return true;
}

std::shared_ptr<ILogger> RootLogger::setLogger(const std::string& name,
                                               std::shared_ptr<ILogger> impl) {
  std::shared_ptr<ILogger> value;
  std::map<std::string, std::shared_ptr<ILogger>>::iterator it =
      this->loggers.find(name);
  if (it != this->loggers.end()) {
    value = it->second;
  }
  this->loggers[name] = impl;
  if (impl) impl->setLevel(this->level);
  return value;
}

std::shared_ptr<ILogger> RootLogger::removeLogger(const std::string& name) {
  std::shared_ptr<ILogger> value;
  std::map<std::string, std::shared_ptr<ILogger>>::iterator it =
      this->loggers.find(name);
  if (it != this->loggers.end()) {
    value = it->second;
    this->loggers.erase(name);
  }
  return value;
}

std::map<std::string, std::shared_ptr<ILogger>> RootLogger::removeAll() {
  std::map<std::string, std::shared_ptr<ILogger>> cache = this->loggers;
  this->loggers.clear();
  return cache;
}

}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik