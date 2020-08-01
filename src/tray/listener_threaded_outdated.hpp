#pragma once

#include <src/win/windows_headers.hpp>

namespace msw::tray {
class Tray;

class ListenerThreadedOutdated {

  static void file_listen_procedure(const tray::Tray* tray);
  static void file_listen_procedure_threadentry_catches_exceptions(const tray::Tray* tray);


public:
  void thread_listen_for_file_changes(const tray::Tray* tray);

};
} // namespace msw::tray
