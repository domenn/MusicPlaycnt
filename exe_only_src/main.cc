#include <src/win/windows_headers.hpp>

#include <exe_only_src/production_accessor.hpp>
#include <fstream>
#include <processthreadsapi.h>
#include <sago/platform_folders.h>
#include <src/data/legacy_json_importer.hpp>
#include <src/data/pointers_to_globals.hpp>
#include <src/misc/consts.hpp>
#include <src/misc/notification_sink.hpp>
#include <src/misc/spd_logging.hpp>
#include <src/model/app_config.hpp>
#include <src/musicstuff/foo_np_log_parser.hpp>
#include <src/tray/tray.hpp>
#include <src/win/winapi_exceptions.hpp>

extern std::vector<spdlog::sink_ptr> additional_sinks;

msw::model::AppConfig get_or_create_config() {
  try {
    return msw::Serializable::from_file<msw::model::AppConfig>(msw::model::AppConfig::get_path_to_config_file());
  } catch (const msw::exceptions::ErrorCode& err) {
    if (err.is_enoent()) {
      msw::model::AppConfig new_config;
      new_config.set_my_defaults();
      new_config.save_me();
      return new_config;
    }
    throw;
  }
}

template <typename func_t>
class FinalAction {
  func_t f_;

 public:
  explicit FinalAction(func_t&& f) : f_(std::move(f)) {}

  ~FinalAction() {
    SPDLOG_TRACE("Normal exit ... calling the cleanup on {}", reinterpret_cast<const void*>(f_));
    f_();
  }
};

#ifdef _WIN32
int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPWSTR lpCmdLine,
                    _In_ int nShowCmd) {
  SetConsoleOutputCP(CP_UTF8);
  // Todo next: if song is 75% same (only differs on path) we need to warn the user. For example ... I add
  // new, then move it.
  // Todo: Duplicates. For example, I try to add song that already exist. Same title and artist, others may differ. We
  // warn.
#else
int main(int argc, char** argv) {
#endif
  spdl::spdlog_setup(spdl::SpdlogConfig::build()
                         .default_logger_name("NewMusicTrackerCounter")
                         .file_name("NewMusicTrackerCounter.log")
                         .pattern(spdl::SpdlogConfig::PATTERN_ALL_DATA)
                         .level(spdlog::level::trace)
                         .log_to_file(true));

#if defined(DOMEN_WITH_WINTOAST) | defined(DOMEN_WINDOWS_NO_TOAST)  // ;NOTE: other notification impls.
  additional_sinks.emplace_back(std::make_shared<spdl::NotificationSink>());
#else
  static_assert(false, "Missing implementation of warning / notification mechanism!");
#endif  // defined(DOMEN_WITH_WINTOAST) || defined(_MSC_VER)

  FinalAction fa(&spdl::spdlog_teardown);

#if defined(_DLL) && defined(_MSC_VER)
  SetThreadDescription(GetCurrentThread(), L"MainThread");
#endif
  const std::optional<msw::helpers::CmdParse> cmd_parse([]() {
    try {
#ifdef _WIN32
      return std::make_optional<msw::helpers::CmdParse>(GetCommandLineW());
#else
      return std::make_optional<msw::helpers::CmdParse>(argc, argv);
#endif
    } catch (const std::exception& x) {
      SPDLOG_ERROR("Issue: msw::helpers::CmdParse(GetCommandLineW())");
      msw::exceptions::information_for_user(
          x, msw::consts::HEADING_BAD_CMD, msw::exceptions::InformationSeverity::CRITICAL);
      return static_cast<std::optional<msw::helpers::CmdParse>>(std::nullopt);
    }
  }());
  if (!cmd_parse.has_value()) {
    return -1;
  }
  if (cmd_parse->help()) return 0;
  auto cfg = get_or_create_config();
  msw::pg::app_config = &cfg;
  msw::data::ProductionAccessor<msw::model::SongWithMetadata> inst_handled_song(cfg.stored_state_path());
  msw::pg::handled_song = &inst_handled_song;

  msw::data::ProductionAccessor<msw::model::SongList> inst_songlist(cfg.stored_data());
  msw::pg::song_list = &inst_songlist;

  if (cmd_parse->is_listen()) {
    msw::tray::Tray main_tray(hInstance);

    return main_tray.run_message_loop();
  }
  // todo 2 Here I can get song .. and call the procedure (that is otherwise taken care inside message_loop).
  // SPDLOG_INFO(cmd_parse->song_data().artist);

  if (auto thingy = cmd_parse->import_legacy_path(); !thingy.empty()) {
    LegacyJsonImporter li(std::move(thingy));
  }
  return 0;
}

std::string msw::helpers::Utilities::app_folder() { return sago::getConfigHome() + '/' + consts::PROGRAM_NAME_SHORT; }

void msw::musicstuffs::FooNpLogParser::init_lines_reader() {
  reverse_line_reader_.emplace(
      std::make_unique<std::ifstream>(msw::pg::app_config->file_to_listen(), std::ios::binary));
}
