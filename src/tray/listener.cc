#include "listener.hpp"
#include "src/misc/utilities.hpp"
#include "tray.hpp"
#include "src/model/app_config.hpp"
#include "src/win/encoding.hpp"

#include <FileWatcher/FileWatcher.h>
#include <spdlog/spdlog.h>

namespace msw {
/// Processes a file action
class UpdateListener : public FW::FileWatchListener {
public:
  UpdateListener(tray::Tray* tray, std::string const& file_name);

  void handleFileAction(FW::WatchID watchid,
                        const FW::String& dir,
                        const FW::String& filename,
                        FW::Action action) override;
  tray::Tray* tray_;
  std::wstring file_name_;
};

UpdateListener::UpdateListener(tray::Tray* tray, std::string const& file_name)
  : tray_(tray),
    file_name_(msw::encoding::utf8_to_utf16(file_name.c_str())) {

}

void UpdateListener::handleFileAction(FW::WatchID watchid,
                                      const FW::String& dir,
                                      const FW::String& filename,
                                      FW::Action action) {
  if (filename == file_name_) {
    SPDLOG_INFO("change as needed!");
  }
}
}

msw::tray::Listener::Listener(Tray* tray)
  : tray_(tray) {
  const auto dir = tray->config().file_to_listen();
  const auto [folder_path, filename] = helpers::Utilities::get_parent_folder_and_filename(dir);

  auto folder_path_w = msw::encoding::utf8_to_utf16(folder_path.c_str());

  //HANDLE h_directory = CreateFileW(folder_path_w.c_str(), FILE_LIST_DIRECTORY | GENERIC_READ, 
  // FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
  // NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 
  // )
  //
  UpdateListener* listener = new UpdateListener(tray, filename);
  FW::FileWatcher* fileWatcher = new FW::FileWatcher;
  FW::WatchID watchID = fileWatcher->addWatch(folder_path_w, listener, true);
  while (true) {
    SPDLOG_INFO("Calling update ...");
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    fileWatcher->update();
  }
}
