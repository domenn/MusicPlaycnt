#include <sago/platform_folders.h>

#include <exe_only_src/production_accessor.hpp>
#include <external/cxxopt.hpp>
#include <fstream>
#include <src/data/pointers_to_globals.hpp>
#include <src/misc/consts.hpp>
#include <src/model/song.hpp>
#include <src/win/windows_headers.hpp>

#include "misc/spd_logging.hpp"
#include "misc/utilities.hpp"
#include "model/app_config.hpp"
#include "musicstuff/foo_np_log_parser.hpp"
#include "tray/listener.hpp"
#include "tray/tray.hpp"
#include "win/winapi_exceptions.hpp"

namespace msw::pg {
data::Accessor<msw::model::SongList>* song_list;
data::Accessor<msw::model::SongWithMetadata>* handled_song;
}  // namespace msw::pg

msw::model::AppConfig get_or_create_config() {
  try {
    return msw::Serializable::from_file<msw::model::AppConfig>(msw::model::AppConfig::get_path_to_config_file());
  } catch (const msw::exceptions::ErrorCode& err) {
    if (err.is_enoent()) {
      const auto path =
          msw::helpers::Utilities::file_in_app_folder_with_creating_app_folder(msw::consts::CONFIG_FILENAME);
      msw::model::AppConfig new_config;
      new_config.set_my_defaults();
      new_config.serialize(path);
      return new_config;
    }
    throw;
  }
}
#ifdef _WIN32
int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPWSTR lpCmdLine,
                    _In_ int nShowCmd) {
  SetConsoleOutputCP(CP_UTF8);
#else
int main(int argc, char** argv) {
#endif
  spdl::spdlog_setup(spdl::SpdlogConfig::build()
                         .default_logger_name("NewMusicTrackerCounter")
                         .file_name("NewMusicTrackerCounter.log")
                         .pattern(spdl::SpdlogConfig::PATTERN_ALL_DATA)
                         .log_to_file(true));
#ifdef _DLL
  SetThreadDescription(GetCurrentThread(), L"MainThread");
#endif
#ifdef _WIN32
  msw::helpers::CmdParse cmd_parse(GetCommandLineW());
#else
  msw::helpers::CmdParse cmd_parse(argc, argv);
#endif
  auto cfg = get_or_create_config();

  msw::data::ProductionAccessor<msw::model::SongWithMetadata> inst_handled_song(cfg, cfg.stored_state_path());
  msw::pg::handled_song = &inst_handled_song;

  if (cmd_parse.is_listen()) {
    msw::tray::Tray main_tray(hInstance, cfg);

    return main_tray.run_message_loop();
  }
  // todo 2 Here I can get song .. and call the procedure (that is otherwise taken care inside message_loop).
  SPDLOG_INFO(cmd_parse.song_data().artist);

  return 0;
}

std::string msw::helpers::Utilities::app_folder() { return sago::getConfigHome() + '/' + consts::PROGRAM_NAME_SHORT; }

std::string msw::musicstuffs::FooNpLogParser::LineGetter::next() {
  constexpr int LINES_TO_READ = 2;
  ifstream_.seekg(0, std::ios_base::end);
  size_t size = static_cast<size_t>(ifstream_.tellg());
  int newlineCount = 0;
  while (ifstream_ && buffer_.size() != size && newlineCount < LINES_TO_READ) {
    buffer_.resize(std::min(buffer_.size() + LINE_SIZE_STEP, size));
    ifstream_.seekg(-static_cast<std::streamoff>(buffer_.size()), std::ios_base::end);
    ifstream_.read(buffer_.data(), buffer_.size());
    newlineCount = std::count(buffer_.begin(), buffer_.end(), '\n');
  }

  auto rightmost_newline = std::next(std::find(buffer_.rbegin(), buffer_.rend(), '\n'));
  if (std::distance(buffer_.rbegin(), rightmost_newline) > 3) {
    // no newline at the end
    return {rightmost_newline.base() + 1, buffer_.end()};
  }
  auto next_newline = std::find(rightmost_newline, buffer_.rend(), '\n');
  return std::string(next_newline.base(), rightmost_newline.base());
}

msw::musicstuffs::FooNpLogParser::LineGetter::LineGetter(std::ifstream ifstream1) : ifstream_(std::move(ifstream1)) {
  buffer_.reserve(LINE_SIZE_STEP + 1);
  ifstream_.seekg(0, std::ios_base::end);
  file_size_ = ifstream_.tellg();
}

msw::musicstuffs::FooNpLogParser::LineGetter msw::musicstuffs::FooNpLogParser::lines() const {
  return std::ifstream(app_config_.file_to_listen(), std::ios::binary);
}
