#include <src/win/windows_headers.hpp>

#include <src/misc/notification_sink.hpp>
#include <src/win/encoding.hpp>

spdl::NotificationSink::~NotificationSink() = default;

void spdl::NotificationSink::log(const spdlog::details::log_msg& msg) {
  if (msg.level >= spdlog::level::warn) {
    auto null_terminated(msw::encoding::utf8_to_utf16(std::string(msg.payload.data(), msg.payload.size()).c_str()));
    MessageBoxW(nullptr, null_terminated.c_str(), L"MusicThing2 What Now?", MB_OK | MB_ICONEXCLAMATION);
  }
}

void spdl::NotificationSink::flush() {}

void spdl::NotificationSink::set_pattern(const std::string& pattern) {}

void spdl::NotificationSink::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) {}

spdl::NotificationSink::NotificationSink() = default;