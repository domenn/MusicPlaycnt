#include <Windows.h>
#include <fmt/format.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Log.h>

#include <iostream>

int main() {
  std::cout << "PLEASE ATTACH DEBUGGER TO PID: " << GetCurrentProcessId() << std::endl;
  int waits = 0;
  while (!::IsDebuggerPresent()) {
    ::Sleep(100);  // to avoid 100% CPU load
    // Wait for max 45 seconds.
    if (waits++ == 450) {
      break;
    }
  }

  static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::Severity::debug, &consoleAppender);
  PLOG_VERBOSE << "This is a VERBOSE message";
  PLOG_DEBUG << "This is a DEBUG message";
  PLOG_INFO << "This is an INFO message";
  PLOG_WARNING << "This is a WARNING message";
  PLOG_ERROR << "This is an ERROR message";
  PLOG_FATAL << "This is a FATAL message";
  return 0;
}
