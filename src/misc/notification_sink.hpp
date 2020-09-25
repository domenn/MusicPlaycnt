#pragma once

#include <src/misc/custom_include_spdlog.hpp>

#if defined(DOMEN_WITH_WINTOAST)
#include <wintoast/wintoastlib.h>

namespace spdl::wintoast {
class DoNothingHandler : public WinToastLib::IWinToastHandler {
 public:

  long long id;

  void toastActivated() const override {
    // SPDLOG_TRACE("Acti");
  }
  void toastDismissed(WinToastDismissalReason state) const override {
    // SPDLOG_TRACE("down");
  }
  void toastFailed() const override {
    // SPDLOG_TRACE("Fa");
  }
  void toastActivated(int actionIndex) const override {
    // SPDLOG_TRACE("Activa");
  }

  ~DoNothingHandler() override = default;
};
}  // namespace spdl::wintoast

#endif  // defined(DOMEN_WITH_WINTOAST)

namespace spdl {
class NotificationSink : public spdlog::sinks::sink {
  bool is_up_{false};

#if defined(DOMEN_WITH_WINTOAST)
  WinToastLib::WinToastTemplate win_toast_template_{};
#endif

 public:
  NotificationSink();

  ~NotificationSink() override;

  void log(const spdlog::details::log_msg& msg) override;
  void flush() override;
  void set_pattern(const std::string& pattern) override;
  void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override;
};
}  // namespace spdl