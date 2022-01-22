#pragma once

namespace ggolbik {
namespace opcua {
namespace logging {

/**
 * @brief Representation of log level.
 */
enum class Level {
  off,
  trace,
  debug,
  info,
  security,
  warn,
  error,
  fatal,
};

}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik