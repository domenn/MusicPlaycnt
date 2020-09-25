#include <src/win/windows_headers.hpp>

#include <src/misc/consts.hpp>
#include <src/misc/notification_sink.hpp>
#include <src/win/encoding.hpp>
#include <src/win/windows_error_format.hpp>

using namespace WinToastLib;

std::mutex my_awsome_mutex;
//std::vector<int64_t> displayed_items{};
//std::vector<int64_t> displayed_items_unsafe{};
//
//std::vector<int64_t>* safer_displayed_items{};

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

spdl::NotificationSink::NotificationSink() : win_toast_template_(make_templ()) {
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

spdl::NotificationSink::~NotificationSink() {
  if (is_up_) {
    std::lock_guard<std::mutex> lck(my_awsome_mutex);
    if (is_up_) {
      is_up_ = false;
      auto* instt = WinToast::instance();
      //for (auto id : displayed_items) {
      //  instt->hideToast(id);
      //}
      instt->clear();
      // displayed_items_.clear();
    }
  }
}

void spdl::NotificationSink::log(const spdlog::details::log_msg& msg) {
  BAIL_IF_DOWN

  std::string null_terminated(msg.payload.data(), msg.payload.size());
  win_toast_template_.setTextField(

      msw::encoding::utf8_to_utf16((std::string(msg.logger_name.data(), msg.logger_name.size()) + " - " +
                                    spdlog::level::to_string_view(msg.level).data())
                                       .c_str())
          .c_str(),
      WinToastTemplate::FirstLine);
  win_toast_template_.setTextField(msw::encoding::utf8_to_utf16(null_terminated.data()), WinToastTemplate::SecondLine);
  auto * handler_inst = new wintoast::DoNothingHandler;
  auto itm = WinToast::instance()->showToast(win_toast_template_, handler_inst);
  if (!itm) {
    std::lock_guard<std::mutex> lck(my_awsome_mutex);
    is_up_ = false;
    const auto er_cd = GetLastError();
    SPDLOG_WARN("Unable to show the wintoast for log! {};{}",
                er_cd,
                msw::windows::format_windows_error(er_cd, "WinToast::instance()->showToast"));
    is_up_ = true;
  }
  else {
    handler_inst->id = itm;
    // displayed_items.push_back(itm);
  }
}

void spdl::NotificationSink::flush() { BAIL_IF_DOWN }

void spdl::NotificationSink::set_pattern(const std::string& pattern) { BAIL_IF_DOWN }

void spdl::NotificationSink::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) { BAIL_IF_DOWN }
