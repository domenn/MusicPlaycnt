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
#include "tray/tray.hpp"
#include "win/winapi_exceptions.hpp"

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
  msw::pg::app_config = &cfg;

  msw::data::ProductionAccessor<msw::model::SongWithMetadata> inst_handled_song(cfg, cfg.stored_state_path());
  msw::pg::handled_song = &inst_handled_song;

  if (cmd_parse.is_listen()) {
    msw::tray::Tray main_tray(hInstance);

    return main_tray.run_message_loop();
  }
  // todo 2 Here I can get song .. and call the procedure (that is otherwise taken care inside message_loop).
  SPDLOG_INFO(cmd_parse.song_data().artist);

  return 0;
}

std::string msw::helpers::Utilities::app_folder() { return sago::getConfigHome() + '/' + consts::PROGRAM_NAME_SHORT; }

void msw::musicstuffs::FooNpLogParser::init_lines_reader() {
  reverse_line_reader_.emplace(
      std::make_unique<std::ifstream>(msw::pg::app_config->file_to_listen(), std::ios::binary));
}
