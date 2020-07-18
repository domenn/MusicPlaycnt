
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

#include <src/model/song.hpp>

#include "misc/spd_logging.hpp"
#include "ostream_logging.hpp"

int main(int argc, char** argv) {
  spdl::spdlog_setup(spdl::SpdlogConfig::build()
                         .default_logger_name("NewMusicTrackerCounter")
                         .file_name("NewMusicTrackerCounter.log")
                         .log_to_file(true)
                         .pattern(spdl::SpdlogConfig::PATTERN_ALL_DATA)
                         .log_to_file(true));

  msw::model::Song::testing_create_restore();

  SPDLOG_WARN("one {}", 33);
  SPDLOG_WARN("another ... haha");
  SPDLOG_OSR_DEBUG("lol ..." << 33 << " " << 5.551 << ' ' << std::endl << "NEWLINE!!!!!!!");
  SPDLOG_OSR_WARN("A ... warning.RLY" << "lol ..." << 33 << " " << 5.551 << ' ' << "NEWLINE!!!!!!!");

  return 0;
}