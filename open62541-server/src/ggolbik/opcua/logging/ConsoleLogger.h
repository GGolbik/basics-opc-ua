#pragma once

#include <mutex>
#include <ostream>

#include "Logger.h"

namespace ggolbik {
namespace opcua {
namespace logging {

class ConsoleLogger : public Logger {
 public:
  ConsoleLogger();
  explicit ConsoleLogger(Level level);

 public:              // ILogger methods
  using Logger::log;  // provide Logger implementations of log
  void log(const std::string& tag, Level level, const std::string& category,
           const std::exception& e) override;
  void vlog(const std::string& tag, Level level, const std::string& category,
            const std::string& format, std::va_list args) override;
  void vlog(const std::string& tag, Level level, const std::string& category,
            const std::exception& e, const std::string& format,
            std::va_list args) override;

 private:
  std::ostream& writeStartColor(std::ostream& stream, Level level);
  std::ostream& writeEndColor(std::ostream& stream, Level level);
  std::ostream& writeDateTime(std::ostream& stream);
  std::ostream& writeLevel(std::ostream& stream, Level level);
  std::ostream& writeTag(std::ostream& stream, const std::string& tag);
  std::string stringFormat(const std::string& format, ...);
  std::string vstringFormat(const std::string& format, std::va_list args);

 private:
  std::mutex logMutex;
};

}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik