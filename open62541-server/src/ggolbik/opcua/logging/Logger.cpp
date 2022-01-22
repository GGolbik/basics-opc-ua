#include "Logger.h"

namespace ggolbik {
namespace opcua {
namespace logging {

Logger::Logger(Level level) : level(level) {
  // empty
}

Level Logger::getLevel() const { return this->level; }

void Logger::setLevel(Level level) { this->level = level; }

bool Logger::isLevelEnabled(Level level) const { return this->level <= level; }

void Logger::log(const std::string& tag, Level level,
                 const std::string& category, const std::string& format, ...) {
  if (!this->isLevelEnabled(level)) {
    return;
  }
  va_list args;
  // Name of the last named parameter in the function definition: format
  va_start(args, format);
  this->vlog(tag, level, category, format, args);
  va_end(args);
}

void Logger::log(const std::string& tag, Level level,
                 const std::string& category, const std::exception& e,
                 const std::string& format, ...) {
  if (!this->isLevelEnabled(level)) {
    return;
  }
  va_list args;
  // Name of the last named parameter in the function definition: format
  va_start(args, format);
  this->vlog(tag, level, category, e, format, args);
  va_end(args);
}

}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik