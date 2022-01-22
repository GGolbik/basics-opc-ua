#pragma once

#include <map>     // std::map
#include <memory>  // std::shared_ptr
#include <string>  // std::string

#include "Logger.h"

namespace ggolbik {
namespace opcua {
namespace logging {

class RootLogger : public Logger {
 public:  // constructor/destructor
  RootLogger();
  explicit RootLogger(Level level);
  explicit RootLogger(
      Level level,
      const std::map<std::string, std::shared_ptr<ILogger>>& impls);

 public:              // ILogger methods
  using Logger::log;  // provide Logger implementations of log
  void log(const std::string& tag, Level level, const std::string& category,
           const std::exception& e) override;
  void vlog(const std::string& tag, Level level, const std::string& category,
            const std::string& format, std::va_list args) override;
  void vlog(const std::string& tag, Level level, const std::string& category,
            const std::exception& e, const std::string& format,
            std::va_list args) override;

 public:
  bool addLogger(const std::string& name, std::shared_ptr<ILogger> impl);
  std::shared_ptr<ILogger> setLogger(const std::string& name,
                                     std::shared_ptr<ILogger> impl);
  std::shared_ptr<ILogger> removeLogger(const std::string& name);
  std::map<std::string, std::shared_ptr<ILogger>> removeAll();

 private:
  std::map<std::string, std::shared_ptr<ILogger>> loggers;
};

}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik