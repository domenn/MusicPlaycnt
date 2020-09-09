#include <external/cxxopt.hpp>
#include <src/misc/consts.hpp>
#include <src/model/song.hpp>
#include <src/win/windows_headers.hpp>
//#ifdef _WIN32
//#include <shellapi.h>
//#endif

#include <sago/platform_folders.h>

#include <fstream>

#include "misc/spd_logging.hpp"
#include "misc/utilities.hpp"
#include "model/app_config.hpp"
#include "musicstuff/foo_np_log_parser.hpp"
#include "tray/listener.hpp"
#include "tray/tray.hpp"
#include "win/winapi_exceptions.hpp"
#include <src/data/pointers_to_globals.hpp>

namespace msw::pg {
data::Accessor<msw::model::SongList>* song_list;
data::Accessor<msw::model::SongWithMetadata>* handled_song;
}

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
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
  SetConsoleOutputCP(CP_UTF8);
#else
int main(int argc, char** argv) {
#endif
  spdl::spdlog_setup(spdl::SpdlogConfig::build()
                     .default_logger_name("NewMusicTrackerCounter")
                     .file_name("NewMusicTrackerCounter.log")
                     .pattern(spdl::SpdlogConfig::PATTERN_ALL_DATA)
                     .log_to_file(true));

  // msw::pg::provider = 
#ifdef _DLL
  SetThreadDescription(GetCurrentThread(), L"MainThread");
#endif
#ifdef _WIN32
  msw::helpers::CmdParse cmd_parse(GetCommandLineW());
#else
  msw::helpers::CmdParse cmd_parse(argc, argv);
#endif
  auto cfg = get_or_create_config();
  if (cmd_parse.is_listen()) {
    msw::tray::Tray main_tray(hInstance, cfg);

    return main_tray.run_message_loop();
  }

  SPDLOG_INFO(cmd_parse.song_data().artist);

  return 0;
}

std::string msw::helpers::Utilities::app_folder() { return sago::getConfigHome() + '/' + consts::PROGRAM_NAME_SHORT; }

std::string msw::musicstuffs::FooNpLogParser::extract_last_line() const {
  constexpr int LINE_SIZE_STEP = 150;
  constexpr int LINES_TO_READ = 2;
  std::ifstream source(app_config_.file_to_listen(), std::ios::binary);
  source.seekg(0, std::ios_base::end);
  size_t size = static_cast<size_t>(source.tellg());
  std::vector<char> buffer;
  int newlineCount = 0;
  while (source && buffer.size() != size && newlineCount < LINES_TO_READ) {
    buffer.resize(std::min(buffer.size() + LINE_SIZE_STEP, size));
    source.seekg(-static_cast<std::streamoff>(buffer.size()), std::ios_base::end);
    source.read(buffer.data(), buffer.size());
    newlineCount = std::count(buffer.begin(), buffer.end(), '\n');
  }

  auto rightmost_newline = std::next(std::find(buffer.rbegin(), buffer.rend(), '\n'));
  if (std::distance(buffer.rbegin(), rightmost_newline) > 3) {
    // no newline at the end
    return {rightmost_newline.base() + 1, buffer.end()};
            }
            auto next_newline = std::find(rightmost_newline, buffer.rend(), '\n');
            return std::string(next_newline.base(), rightmost_newline.base());
            }
