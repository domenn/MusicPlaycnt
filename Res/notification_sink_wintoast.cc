#include <src/win/windows_headers.hpp>

#include <src/misc/consts.hpp>
#include <src/misc/notification_sink.hpp>
#include <src/win/encoding.hpp>
#include <src/win/windows_error_format.hpp>
#include <wintoast/wintoastlib.h>

using namespace WinToastLib;

std::mutex my_awsome_mutex;

class DoNothingHandler : public IWinToastHandler {
 public:
  void toastActivated() const override{};
  void toastDismissed(WinToastDismissalReason state) const override{};
  void toastFailed() const override{};
  void toastActivated(int actionIndex) const override{};
} do_nothing_handler;

#define BAIL_IF_DOWN \
  if (!is_up_) {     \
    return;          \
  }

auto make_templ() {
  WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text02);
  templ.setDuration(WinToastTemplate::System);
  templ.setAudioOption(WinToastTemplate::Silent);
  return templ;
}

WinToastTemplate global_templ(make_templ());

spdl::NotificationSink::NotificationSink() {
  WinToast::instance()->setAppName(msw::consts::PROGRAM_NAME_SHORTW);
  const auto aumi = WinToast::configureAUMI(
      L"Domen", msw::consts::PROGRAM_NAME_SHORTW, msw::consts::CLI_PROGRAM_DESCTIPTIONW, L"20200924");
  WinToast::instance()->setAppUserModelId(aumi);

  if (!WinToast::instance()->initialize()) {
    SPDLOG_WARN("Could not initialize WinToast!");
  } else {
    is_up_ = true;
  }
}

void spdl::NotificationSink::log(const spdlog::details::log_msg& msg) {
  BAIL_IF_DOWN

  std::string null_terminated(msg.payload.data(), msg.payload.size());
  global_templ.setTextField(

      msw::encoding::utf8_to_utf16((std::string(msg.logger_name.data(), msg.logger_name.size()) + " - " +
                                    spdlog::level::to_string_view(msg.level).data())
                                       .c_str())
          .c_str(),
      WinToastTemplate::FirstLine);
  global_templ.setTextField(msw::encoding::utf8_to_utf16(null_terminated.data()), WinToastTemplate::SecondLine);

  if (!WinToast::instance()->showToast(global_templ, &do_nothing_handler)) {
    std::lock_guard<std::mutex> lck(my_awsome_mutex);
    is_up_ = false;
    const auto er_cd = GetLastError();
    SPDLOG_WARN("Unable to show the wintoast for log! {};{}",
                er_cd,
                msw::windows::format_windows_error(er_cd, "WinToast::instance()->showToast"));
    is_up_ = true;
  }
}

void spdl::NotificationSink::flush() { BAIL_IF_DOWN }

void spdl::NotificationSink::set_pattern(const std::string& pattern) { BAIL_IF_DOWN }

void spdl::NotificationSink::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) { BAIL_IF_DOWN }
