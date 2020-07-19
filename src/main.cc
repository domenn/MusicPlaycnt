//#include <plog/Log.h>
//
//#include "misc/plog_util.hpp"
//#include "test_plog_wtf/plog_util.hpp"
//#include "Windows.h"
//
// void MyTrace(int line, const char *fileName, const wchar_t *msg, ...) {
//  va_list args;
//  wchar_t buffer[256] = {0};
//  const auto converted = CustomTextFormater::filename_rightmost(fileName, true);
//  swprintf_s(buffer, L"%s(%d) : ", converted.c_str(), line);
//  OutputDebugStringW(buffer);
//  // retrieve the variable arguments
//  va_start(args, msg);
//  vswprintf_s(buffer, msg, args);
//  OutputDebugStringW(buffer);
//  OutputDebugStringW(L"\n");
//  va_end(args);
//}
//
//#define MY_TRACE(msg, ...) MyTrace(__LINE__, __FILE__, msg, __VA_ARGS__)
//#define MY_TRACE_NOV(msg) MyTrace(__LINE__, __FILE__, msg)
//
// int main() {
//  //  std::cout << "PLEASE ATTACH DEBUGGER TO PID: " << GetCurrentProcessId() << std::endl;
//  //  int waits = 0;
//  //  while (!::IsDebuggerPresent()) {
//  //    ::Sleep(100);  // to avoid 100% CPU load
//  //    // Wait for max 45 seconds.
//  //    if (waits++ == 450) {
//  //      break;
//  //    }
//  //  }
//
//  plog::Severity log_lvl =
//#ifndef NDEBUG
//      plog::Severity::verbose
//#else
//      plog::Severity::debug
//#endif
//      ;
//
//  PlogUtil::setup_logger("MusicPlaycnt/logs.log", true, log_lvl);
//  // PlogUtil::setup_logger("domsLoggs", true, log_lvl);
//
//  OutputDebugStringW(L"..\\src\\main.cpp(47): error my ugly msg\n");
//
//  return 0;
//  MY_TRACE_NOV(L"Shall be clickable in msvc thingy");
//
//  log_experimental1();
//
//  PLOG_VERBOSE << "This is a VERBOSE message";
//  PLOG_DEBUG << "This is a DEBUG message";
//  PLOG_INFO << "This is an INFO message";
//  PLOG_WARNING << "This is a WARNING message";
//  // PLOG_ERROR << "This is an ERROR message with pf: " << sago::getDocumentsFolder();
//  PLOG_FATAL << "This is a FATAL message";
//  return 0;
//}

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
#include "ostream_logging.hpp"

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
      std::string cmdLineA = GetCommandLineA();
      std::wstring cmdLineA_w = msw::encoding::utf8_to_utf16(cmdLineA.c_str());
      std::wstring wins_cmdline_one = GetCommandLineW();
      std::wstring wins_cmdline_two = wins_cmdline;

      SPDLOG_INFO("Received commandline (ATTEMPTS)\n{} {}\n{} {}\n{} {}\n{} {}\n",
                   " cmdLineA(string)", cmdLineA,
                   " cmdLineA_w", msw::encoding::utf16_to_utf8(cmdLineA_w),
                   " wins_cmdline_one", msw::encoding::utf16_to_utf8(wins_cmdline_one),
                   " wins_cmdline_two", msw::encoding::utf16_to_utf8(wins_cmdline_two)

                   );

      wchar_t** output = CommandLineToArgvW(wins_cmdline_one.c_str(), &num_args);
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
  const cxxopts::Options options_;

  [[nodiscard]] cxxopts::Options create_options() const {
    cxxopts::Options options(msw::consts::PROGRAM_NAME_SHORT, msw::consts::CLI_PROGRAM_DESCTIPTION);
    options.add_options()("l,listen", "Listener for file. Tray app.");
    options.parse(const_cast<int&>(argc_argv_.argc_), const_cast<char**&>(argc_argv_.argv_));
    return options;
  }

 public:
  CmdParse(const int argc, const char** argv) : argc_argv_{argc, argv}, options_(create_options()) {}
#ifdef _WIN32
  CmdParse(const wchar_t* lpCmdLine) : argc_argv_(ArgcArgv::from_wstring(lpCmdLine)), options_(create_options()) {}
#endif
};

//
// class CmdParse1 {
//#ifdef _WIN32
//  std::string wins_cmdline;
//  static std::pair<int, char**> windows_version(const wchar_t* lpCmdLine){
//     msw::encoding::utf16_to_utf8(lpCmdLine);
//  }
//#endif
//  static cxxopts::Options create_parse_options( std::pair<int, char**> argc_argv){
//
//  }
//  cxxopts::Options options_;
//
// public:
//#ifdef _WIN32
//  CmdParse1(const wchar_t* lpCmdLine) {
//
//  }
//#endif
//
//  //  static cxxopts::Options build_commandline_options(){
////    cxxopts::Options options(msw::consts::PROGRAM_NAME_SHORT, msw::consts::CLI_PROGRAM_DESCTIPTION);
////    options.add_options()
////        ("l,listen", "Listener for file. Tray app.");
////    return options;
////  }
////  static cxxopts::Options opts() {
////    static cxxopts::Options internal_opts = build_commandline_options();
////    internal_opts.pa
////  }
//};

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
  spdl::spdlog_setup(spdl::SpdlogConfig::build()
                         .default_logger_name("NewMusicTrackerCounter")
                         .file_name("NewMusicTrackerCounter.log")
                         .log_to_file(true)
                         .pattern(spdl::SpdlogConfig::PATTERN_ALL_DATA)
                         .log_to_file(true));
  CmdParse cmd_parse_(lpCmdLine);

  msw::model::Song::testing_create_restore();
  msw::model::SongList::example_songlists();

  SPDLOG_WARN("one {}", 33);
  SPDLOG_WARN("another ... haha");
  SPDLOG_OSR_DEBUG("lol ..." << 33 << " " << 5.551 << ' ' << std::endl << "NEWLINE!!!!!!!");
  SPDLOG_OSR_WARN("A ... warning.RLY"
                  << "lol ..." << 33 << " " << 5.551 << ' ' << "NEWLINE!!!!!!!");

  return 0;
}
