#include "listener_threaded_outdated.hpp"

#include <src/misc/consts.hpp>
#include <thread>

#include "Res/resource.hpp"
#include "src/misc/utilities.hpp"
#include "src/model/app_config.hpp"
#include "src/win/winapi_exceptions.hpp"
#include "tray.hpp"

void msw::tray::ListenerThreadedOutdated::file_listen_procedure(const tray::Tray* tray) {
  const auto dir = tray->config().file_to_listen();
  auto [folder_path, filename] = helpers::Utilities::get_parent_folder_and_filename(dir);


}

void msw::tray::ListenerThreadedOutdated::file_listen_procedure_threadentry_catches_exceptions(const tray::Tray* tray) {
  try {
    file_listen_procedure(tray);
  } catch (const msw::exceptions::ApplicationError& ae) {
    SPDLOG_CRITICAL("Thread died with {}\n{}{}",
                    typeid(ae).name(),
                    ae.what(),
                    ae.stacktrace()
        );
    tray->send_windows_message(CUSTOM_THREAD_IS_DOWN);
  } catch (const std::exception& any_x) {
    SPDLOG_CRITICAL("Thread died with uncaught exception {}\n{}",
                    typeid(any_x).name(),
                    any_x.what()
        );
    tray->send_windows_message(CUSTOM_THREAD_IS_DOWN);
  }
}

void msw::tray::ListenerThreadedOutdated::thread_listen_for_file_changes(const tray::Tray* tray) {
  std::thread inst_thread(file_listen_procedure_threadentry_catches_exceptions, tray);
#ifdef _DLL
  SetThreadDescription(inst_thread.native_handle(),
                       msw::consts::LISTENER_THREAD_NAMEW);
#endif
  inst_thread.detach();
}
