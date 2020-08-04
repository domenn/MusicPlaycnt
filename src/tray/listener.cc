#include <src/win/windows_headers.hpp>

#include "listener.hpp"
#include "src/misc/utilities.hpp"
#include "tray.hpp"
#include "src/model/app_config.hpp"
#include "src/win/encoding.hpp"

#include <spdlog/spdlog.h>
#include "src/win/winapi_exceptions.hpp"
#include "Res/resource.hpp"

VOID CALLBACK msw::tray::Listener::my_wait_or_timer_cb(
    _In_ PVOID lpParameter,
    _In_ BOOLEAN TimerOrWaitFired
    ) {
  msw::tray::Listener* l = (msw::tray::Listener*)lpParameter;
  UnregisterWait(l->async_change_handle_);
  SPDLOG_INFO("Th: {},{} sending msg to window.", GetCurrentThreadId(), helpers::Utilities::get_thread_description());
  l->tray_->send_windows_message(CUSTOM_CHANGE_NOTIFY);
  SPDLOG_INFO("CB!!!");
}

msw::tray::Listener::Listener(Tray* tray)
  : tray_(tray) {
  const auto discardable = msw::helpers::Utilities::get_parent_folder_and_filename(
      tray->config().file_to_listen()
      );
  std::tie(folder_, file_) = std::make_pair(
      encoding::utf8_to_utf16(discardable.first.c_str()),
      encoding::utf8_to_utf16(discardable.second.c_str())
      );
  SPDLOG_TRACE("Listener will setup: {}\\;\\{}", discardable.first, discardable.second);
  change_handle_ = FindFirstChangeNotification(
      folder_.c_str(),
      FALSE,
      FILE_NOTIFY_CHANGE_LAST_WRITE);

  if (change_handle_ == INVALID_HANDLE_VALUE || change_handle_ == nullptr) {
    throw msw::exceptions::WinApiError(GetLastError(), "FindFirstChangeNotification", MSW_TRACE_ENTRY_CREATE);
  }
  listen<true>();
}

template <bool first>
void msw::tray::Listener::listen() {
  if constexpr (!first) {
    if (!FindNextChangeNotification(change_handle_)) {
      throw msw::exceptions::WinApiError(GetLastError(), "FindNextChangeNotification", MSW_TRACE_ENTRY_CREATE);
    }
  } else {
    SPDLOG_INFO("Th: {},{} LISTENER UPPING.", GetCurrentThreadId(), helpers::Utilities::get_thread_description());
  }
  if (!RegisterWaitForSingleObject(&async_change_handle_,
                                   change_handle_,
                                   &my_wait_or_timer_cb,
                                   this,
                                   INFINITE,
                                   WT_EXECUTEONLYONCE)) {
    throw msw::exceptions::WinApiError(GetLastError(), "RegisterWaitForSingleObject", MSW_TRACE_ENTRY_CREATE);
  }
}

template void msw::tray::Listener::listen<true>();
template void msw::tray::Listener::listen<false>();
