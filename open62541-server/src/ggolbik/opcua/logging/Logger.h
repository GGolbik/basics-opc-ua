#pragma once

#include "ILogger.h"

namespace ggolbik {
namespace opcua {
namespace logging {

class Logger : public ILogger {
 protected:
  explicit Logger(Level level);

 public:
  Level getLevel() const override;
  void setLevel(Level level) override;
  bool isLevelEnabled(Level level) const override;
  using ILogger::log;
  void log(const std::string& tag, Level level, const std::string& category, const std::string& format,
           ...) override;
  void log(const std::string& tag, Level level, const std::string& category, const std::exception& e,
           const std::string& format, ...) override;

 protected:
  Level level = Level::off;
};
}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik