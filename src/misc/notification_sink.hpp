#pragma once

#include <src/misc/custom_include_spdlog.hpp>

namespace spdl {

class NotificationSink : public spdlog::sinks::sink {
  bool is_up_{false};

 public:
  NotificationSink();

  void log(const spdlog::details::log_msg& msg) override;
  void flush() override;
  void set_pattern(const std::string& pattern) override;
  void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override;
};
}  // namespace spdl