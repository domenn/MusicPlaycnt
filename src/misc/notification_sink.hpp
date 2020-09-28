#pragma once

#include <src/misc/custom_include_spdlog.hpp>
#include <src/misc/utiltime.hpp>

#if defined(DOMEN_WITH_WINTOAST)
#include <wintoast/wintoastlib.h>

namespace spdl::wintoast {
class DomenWithWinToastHandler : public WinToastLib::IWinToastHandler {
  const std::wstring message_content_;
  const std::chrono::time_point<std::chrono::system_clock> time_;

 public:
  long long id{};

  void toastActivated() const override;

  void toastDismissed(WinToastDismissalReason state) const override {
    //  SPDLOG_INFO("down {}", state);
  }
  void toastFailed() const override {
    //  SPDLOG_INFO("Fa");
  }
  void toastActivated(int actionIndex) const override {
    //  SPDLOG_INFO("Activa 11_ {}", actionIndex);
  }

  ~DomenWithWinToastHandler() override = default;
  explicit DomenWithWinToastHandler(std::wstring&& message_content,
                                    std::chrono::time_point<std::chrono::system_clock> time)
      : message_content_(std::move(message_content)), time_(std::move(time)) {}

  [[nodiscard]] const wchar_t* message_content() const { return message_content_.c_str(); }
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