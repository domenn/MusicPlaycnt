#include "spdlog_colorful_sink_factory_singleton.hpp"

#include <iostream>

#ifdef _WIN32
#include "../win/windows_headers.hpp"

enum class WIN_FG_COLORS : WORD {
  FG_BLACK = 0,
  FG_BLUE = 1,
  FG_GREEN = 2,
  FG_CYAN = 3,
  FG_RED = 4,
  FG_MAGENTA = 5,
  FG_BROWN = 6,
  FG_LIGHTGRAY = 7,
  FG_GRAY = 8,
  FG_LIGHTBLUE = 9,
  FG_LIGHTGREEN = 10,
  FG_LIGHTCYAN = 11,
  FG_LIGHTRED = 12,
  FG_LIGHTMAGENTA = 13,
  FG_YELLOW = 14,
  FG_WHITE = 15
} FG_COLORS;
#endif

class AnsiColor {
 public:
  constexpr static const char LIGHT_GRAY[] = "\033[0;37m";
};

namespace spdl::internal {

#ifdef _WIN32

class WindowsNativeColorfulSinkFactory : public spdl::internal::ColorfulSinkFactorySingleton {
  spdlog::sink_ptr get_sink() override;
};

class AnsiWindowsColorSinkFactory : public spdl::internal::ColorfulSinkFactorySingleton {
  spdlog::sink_ptr get_sink() override;
};
}  // namespace spdl::internal

spdl::internal::ColorfulSinkFactorySingleton* spdl::internal::ColorfulSinkFactorySingleton::inst() {
  static std::unique_ptr<spdl::internal::ColorfulSinkFactorySingleton> internal_inst = make_the_inst();
  return internal_inst.get();
}

bool test_is_windows_cmd() {
// #ifdef _WIN32
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
  if (hConsole == INVALID_HANDLE_VALUE) {
    return false;
    // std::cout << d_common::windows::format_windows_error(GetLastError(), "GetStdHandle") << std::endl;
  }
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  return GetConsoleScreenBufferInfo(hConsole, &csbi) != 0;
//#else
//  return false;
//#endif
  //  auto colors = SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
  //  if (colors) {
  //    std::cout << "SetConsoleTextAttribute for color, no error.\n";
  //  } else {
  //    std::cout << d_common::windows::format_windows_error(GetLastError(), "SetConsoleTextAttribute") << std::endl;
  //  }
}

std::unique_ptr<spdl::internal::ColorfulSinkFactorySingleton>
spdl::internal::ColorfulSinkFactorySingleton::make_the_inst() {
  if (test_is_windows_cmd()) {
    return std::make_unique<WindowsNativeColorfulSinkFactory>();
  } else {
    return std::make_unique<AnsiWindowsColorSinkFactory>();
  }
}

namespace spdl::internal {
spdlog::sink_ptr WindowsNativeColorfulSinkFactory::get_sink() {
  auto c_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_st>();
  c_sink->set_color(spdlog::level::info, static_cast<WORD>(WIN_FG_COLORS::FG_LIGHTGRAY));
  c_sink->set_color_mode(spdlog::color_mode::always);
  return c_sink;
}

spdlog::sink_ptr AnsiWindowsColorSinkFactory::get_sink() {
  auto c_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_st>();
  c_sink->set_color(spdlog::level::info, AnsiColor::LIGHT_GRAY);
  c_sink->set_color_mode(spdlog::color_mode::always);
  return c_sink;
}

#endif

spdlog::sink_ptr spdl::internal::ColorfulSinkFactorySingleton::make_colorful_stdout() {
#ifdef _WIN32
  // return inst()->get_sink();
#else
  auto c_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
  c_sink->set_color(spdlog::level::info, AnsiColor::LIGHT_GRAY);
  return c_sink;
#endif
}

}  // namespace spdl::internal