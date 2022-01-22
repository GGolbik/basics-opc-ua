#include "ConsoleLogger.h"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>

// ANSI color
#if defined(__linux__) || defined(__MACH__)
// the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLDBLACK "\033[1m\033[30m"
#define BOLDRED "\033[1m\033[31m"
#define BOLDGREEN "\033[1m\033[32m"
#define BOLDYELLOW "\033[1m\033[33m"
#define BOLDBLUE "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN "\033[1m\033[36m"
#define BOLDWHITE "\033[1m\033[37m"
#else
#define RESET ""
#define BLACK ""
#define RED ""
#define GREEN ""
#define YELLOW ""
#define BLUE ""
#define MAGENTA ""
#define CYAN ""
#define WHITE ""
#define BOLDBLACK ""
#define BOLDRED ""
#define BOLDGREEN ""
#define BOLDYELLOW ""
#define BOLDBLUE ""
#define BOLDMAGENTA ""
#define BOLDCYAN ""
#define BOLDWHITE ""
#endif

#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <stdexcept>
#include <string>

/**
 * @brief provide same output with the native function in java called
 * currentTimeMillis().
 *
 * Depends on <chrono>
 */
long long currentTimeMillis() {
  auto timeSinceEpoch = std::chrono::system_clock::now().time_since_epoch();
  long long millisecondsSinceEpoch =
      std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceEpoch)
          .count();
  return millisecondsSinceEpoch;
}

namespace ggolbik {
namespace opcua {
namespace logging {

ConsoleLogger::ConsoleLogger() : Logger(Level::info) {
  // empty
}

ConsoleLogger::ConsoleLogger(Level level) : Logger(level) {
  // empty
}

std::string ConsoleLogger::stringFormat(const std::string& format, ...) {
  std::va_list args;
  ::va_start(args, format);
  std::string result = this->vstringFormat(format, args);
  ::va_end(args);
  return result;
}

std::string ConsoleLogger::vstringFormat(const std::string& format,
                                         std::va_list args) {
  va_list argsCopy;
  va_copy(argsCopy, args);
  int size_s = std::vsnprintf(nullptr, 0, format.c_str(), argsCopy) +
               1;  // Extra space for '\0'
  va_end(argsCopy);
  if (size_s <= 0) {
    throw std::runtime_error("Error during formatting.");
  }
  auto size = static_cast<std::size_t>(size_s);
  auto buf = std::make_unique<char[]>(size);
  std::vsnprintf(buf.get(), size, format.c_str(), args);

  return std::string(buf.get(),
                     buf.get() + size - 1);  // We don't want the '\0' inside
}

std::ostream& ConsoleLogger::writeStartColor(std::ostream& stream,
                                             Level level) {
  switch (level) {
    case Level::trace:
      stream << CYAN;
      break;
    case Level::debug:
      // no color
      break;
    case Level::info:
      stream << GREEN;
      break;
    case Level::warn:
      stream << YELLOW;
      break;
    case Level::error:
      stream << RED;
      break;
    case Level::fatal:
      stream << BOLDRED;
      break;
    default:
      break;
  }
  return stream;
}

std::ostream& ConsoleLogger::writeEndColor(std::ostream& stream, Level level) {
  switch (level) {
    case Level::trace:
      stream << RESET;
      break;
    case Level::debug:
      // no color
      break;
    case Level::info:
      stream << RESET;
      break;
    case Level::warn:
      stream << RESET;
      break;
    case Level::error:
      stream << RESET;
      break;
    case Level::fatal:
      stream << RESET;
      break;
    default:
      break;
  }
  return stream;
}

std::ostream& ConsoleLogger::writeDateTime(std::ostream& stream) {
  long long millis = currentTimeMillis();
  std::time_t t = millis / 1000;
  auto tm = *std::localtime(&t);
  stream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  stream << "." << millis % 1000;
  return stream;
}

std::ostream& ConsoleLogger::writeTag(std::ostream& stream,
                                      const std::string& tag) {
  stream << "[" << tag << "]";
  return stream;
}

std::ostream& ConsoleLogger::writeLevel(std::ostream& stream, Level level) {
  int width = std::string("UNKNOWN").length();
  stream << std::setw(width) << std::left;
  switch (level) {
    case Level::trace:
      stream << "TRACE";
      break;
    case Level::debug:
      stream << "DEBUG";
      break;
    case Level::info:
      stream << "INFO";
      break;
    case Level::warn:
      stream << "WARN";
      break;
    case Level::error:
      stream << "ERROR";
      break;
    case Level::fatal:
      stream << "FATAL";
      break;
    default:
      stream << "UNKNOWN";
      break;
  }
  return stream;
}

void ConsoleLogger::log(const std::string& tag, Level level,
                        const std::string& category, const std::exception& e) {
  if (!this->isLevelEnabled(level)) return;
}

void ConsoleLogger::vlog(const std::string& tag, Level level,
                         const std::string& category, const std::exception& e,
                         const std::string& format, std::va_list args) {
  if (!this->isLevelEnabled(level)) return;
}

void ConsoleLogger::vlog(const std::string& tag, Level level,
                         const std::string& category, const std::string& format,
                         std::va_list args) {
  if (!this->isLevelEnabled(level)) return;

  std::lock_guard<std::mutex> guard(this->logMutex);

  std::stringstream buffer;
  // color
  this->writeStartColor(buffer, level);
  // datetime
  this->writeDateTime(buffer);
  buffer << "\t";
  // write tag
  this->writeTag(buffer, tag);
  buffer << "\t";
  // level
  this->writeLevel(buffer, level);
  buffer << "\t";
  // category
  buffer << category;
  buffer << "\t";

  buffer << this->vstringFormat(format, args);
  this->writeEndColor(buffer, level);
  std::cout << buffer.str() << std::endl;
}

}  // namespace logging
}  // namespace opcua
}  // namespace ggolbik