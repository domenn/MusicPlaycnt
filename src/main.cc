#include <external/cxxopt.hpp>
#include <src/misc/consts.hpp>
#include <src/model/song.hpp>
#include <src/model/song_list.hpp>
#include <src/win/windows_headers.hpp>
#ifdef _WIN32
#include <shellapi.h>

#include <src/win/encoding.hpp>
#endif

#include "misc/spd_logging.hpp"
#include "tray/tray.hpp"
#include "model/app_config.hpp"

class CmdParse {
  class ArgcArgv {
  public:
    const int argc_;
    const char** argv_;
#ifdef _WIN32
    const std::vector<char> raw_data_{};
    const std::vector<const char*> pointers_{};
#endif

    static ArgcArgv from_wstring(const wchar_t* wins_cmdline) {
      int num_args;
      std::vector<char> raw_data;
      std::vector<const char*> pointers;

      SPDLOG_TRACE("Received commandline {}", wins_cmdline);

      wchar_t** output = CommandLineToArgvW(wins_cmdline, &num_args);
      pointers.push_back(nullptr);
      for (int i = 0; i < num_args; ++i) {
        std::string temporary_str = msw::encoding::utf16_to_utf8(output[i]);
        std::copy(temporary_str.begin(), temporary_str.end(), std::back_inserter(raw_data));
        raw_data.emplace_back('\0');
        pointers.push_back(pointers[0] + raw_data.size());
      }
      LocalFree(output);
      pointers.pop_back();
      for (auto& pointer : pointers) {
        pointer = (&*raw_data.begin()) + reinterpret_cast<intptr_t>(pointer);
      }
      return ArgcArgv{num_args, pointers.data(), std::move(raw_data), std::move(pointers)};
    }
  };

#ifdef _WIN32
  // const std::string wins_cmdline_{};
#endif
  const ArgcArgv argc_argv_;
  cxxopts::Options options_;
  const cxxopts::ParseResult result_;

  [[nodiscard]] cxxopts::Options create_options() const {
    cxxopts::Options options(msw::consts::PROGRAM_NAME_SHORT, msw::consts::CLI_PROGRAM_DESCTIPTION);
    options.add_options()("l,listen", "Listener for file. Tray app.");
    return options;
  }

public:
  CmdParse(const int argc, const char** argv)
    : argc_argv_{argc, argv},
      options_(create_options()),
      result_(options_.parse(const_cast<int&>(argc_argv_.argc_), const_cast<char**&>(argc_argv_.argv_))) {
  }
#ifdef _WIN32
  CmdParse(const wchar_t* lpCmdLine)
    : argc_argv_(ArgcArgv::from_wstring(lpCmdLine)),
      options_(create_options()),
      result_(options_.parse(const_cast<int&>(argc_argv_.argc_), const_cast<char**&>(argc_argv_.argv_))) {
  }
#endif

  [[nodiscard]] bool is_listen() const {
    return result_["listen"].as<bool>();
  }
};

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
  SetConsoleOutputCP(CP_UTF8);
#else
int main(int argc, char** argv) {
#endif
  spdl::spdlog_setup(spdl::SpdlogConfig::build()
                     .default_logger_name("NewMusicTrackerCounter")
                     .file_name("NewMusicTrackerCounter.log")
                     .log_to_file(true)
                     .pattern(spdl::SpdlogConfig::PATTERN_ALL_DATA)
                     .log_to_file(true));
  CmdParse cmd_parse(GetCommandLineW());

  msw::model::AppConfig::instance().set_file_to_listen(R"(C:\Users\Domen\Everything\complicated_music_tools\foobarLogger\foo_np_log.txt)");
  msw::model::AppConfig::instance().set_library_path(R"(F:\shared_content\muzq\All)");
  msw::model::AppConfig::instance().store_to_file("Playcntv2.config");

  if (cmd_parse.is_listen()) {
    msw::tray::Tray main_tray(hInstance);
    // printreg::ListenerThread::start_registry_listener(&main_tray);
    return main_tray.run_message_loop();
  }

  msw::model::Song::testing_create_restore();
  msw::model::SongList::example_songlists();

  return 0;
}
