#pragma once

#include <src/win/windows_headers.hpp>

#include <string>

namespace msw::tray {
class Tray;

class Listener {
  Tray* tray_;
  std::wstring folder_{};
  std::wstring file_{};
  HANDLE change_handle_{};
  HANDLE async_change_handle_{};

  static VOID CALLBACK my_wait_or_timer_cb(PVOID lpParameter, BOOLEAN TimerOrWaitFired);

 public:
  Listener(Tray* tray);

  template <bool first = false>
  void listen();
};
}  // namespace msw::tray
