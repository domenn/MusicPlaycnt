#pragma once

#include <src/win/windows_headers.hpp>
#include <shellapi.h>


namespace msw {
namespace model {
class AppConfig;
}
}

namespace msw::tray {
class Tray {
  HINSTANCE hinstance_{};
  HMENU menu_to_display_{};
  HWND hwnd_{};

  const model::AppConfig& cfg_;

  NOTIFYICONDATA create_notifyicondata_structure(HICON icon);
  void make_window(int n_cmd_show, HICON h_icon);
  void setup_icon();

  static LRESULT create_menu(Tray* tray);

  static Tray* get_tray_from_window(HWND hwnd);

  void on_exit() const;
  void handle_menu_choice(BOOL clicked) const;
  LRESULT display_tray_menu(HWND hwnd) const;
  LRESULT handle_tray_icon_callback(HWND hwnd,
                                    UINT msg,
                                    WPARAM w_param,
                                    LPARAM l_param);
  static LRESULT wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
  int run_message_loop();
  void send_windows_message(UINT msg) const;
  const model::AppConfig& config() const { return cfg_; }
  Tray(HINSTANCE hinstance, const model::AppConfig& cfg);
};

inline Tray* Tray::get_tray_from_window(HWND hwnd) {
  return reinterpret_cast<Tray*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}
}
