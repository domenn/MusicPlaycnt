#include "listener.hpp"
#include "src/misc/utilities.hpp"
#include "tray.hpp"
#include "src/model/app_config.hpp"
#include "src/win/encoding.hpp"

#include <spdlog/spdlog.h>
#include "src/win/winapi_exceptions.hpp"

LPOVERLAPPED_COMPLETION_ROUTINE LpoverlappedCompletionRoutine;

void LpoverlappedCompletionRoutine1(
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    OVERLAPPED* lpOverlapped
    ) {
  SPDLOG_INFO("The routine!!!");
}

msw::tray::Listener::Listener(Tray* tray)
  : tray_(tray) {
  const auto dir = tray->config().file_to_listen();
  const auto [folder_path, filename] = helpers::Utilities::get_parent_folder_and_filename(dir);

  auto folder_path_w = msw::encoding::utf8_to_utf16(folder_path.c_str());

  HANDLE h_directory = CreateFileW(folder_path_w.c_str(),
                                   FILE_LIST_DIRECTORY | GENERIC_READ,
                                   FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
                                   nullptr,
                                   OPEN_EXISTING,
                                   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                                   nullptr
      );
  if (h_directory == INVALID_HANDLE_VALUE) {
    throw msw::exceptions::WinApiError(GetLastError(),
                                       "CreateFileW",
                                       MSW_TRACE_ENTRY_CREATE,
                                       "Open directory for listening changes");
  }

  FILE_NOTIFY_INFORMATION* file_notify_information = new FILE_NOTIFY_INFORMATION[8];
  const auto fn_size = sizeof(FILE_NOTIFY_INFORMATION) * 8;

  OVERLAPPED* overlapped = new OVERLAPPED{};

  auto read_result = ReadDirectoryChangesW(h_directory,
                                           file_notify_information,
                                           fn_size,
                                           false,
                                           FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES,
                                           nullptr,
                                           overlapped,
                                           &LpoverlappedCompletionRoutine1
      );

  if (!read_result) {
    throw msw::exceptions::WinApiError(GetLastError(),
                                       "ReadDirectoryChangesW",
                                       MSW_TRACE_ENTRY_CREATE,
                                       "Listening changes async");
  }

}
