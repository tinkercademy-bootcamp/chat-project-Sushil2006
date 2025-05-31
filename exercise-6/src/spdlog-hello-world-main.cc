#include <spdlog/spdlog.h>

int main() {
  // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
  // spdlog::set_pattern("[%D %H:%M:%S.%F] [%^%l%$] [%s %!:%#] [%oms] [%ius]% v ");
  spdlog::set_level(spdlog::level::debug);
  // spdlog::cfg::load_env_levels();

  spdlog::info("Welcome to spdlog!");
  spdlog::error("Some error message with arg: {}", 1);

  spdlog::warn("Easy padding in numbers like {:08d}", 12);
  spdlog::critical(
      "Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
  spdlog::info("Support for floats {:03.2f}", 1.23456);
  spdlog::info("Positional args are {1} {0}..", "too", "supported");
  spdlog::info("{:<30}", "left aligned");

  spdlog::set_level(spdlog::level::debug); // Set global log level to debug
  spdlog::debug("This message should be displayed..");

  // Compile time log levels
  // Note that this does not change the current log level, it will only
  // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
  SPDLOG_TRACE("Hello trace message with param {}", 42);
  SPDLOG_DEBUG("What is the difference between these macros and the above?");
  SPDLOG_INFO("Hello info message");
  SPDLOG_WARN("Hello warn message");
  SPDLOG_ERROR("Hello error message");
  SPDLOG_CRITICAL("Hello critical message");

  return 0;
}