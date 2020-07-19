#pragma once

#if _WIN32
#include "../win/windows_headers.hpp"
#include <fcntl.h>

#include "../win/encoding.hpp"

#include <share.h>
#endif

enum class OpenModeWin : int {
  BINARY
#ifdef _WIN32
  = _O_BINARY
#endif
  ,
  TEXT
#ifdef _WIN32
  = _O_TEXT
#endif
};

enum class OpenShareFlag : int {
  DENY_WR
#ifdef _WIN32
  = _SH_DENYWR
#endif
  ,
};

auto cp_open_lw(const char *const fn, int oflag, int pmode = 0, OpenModeWin om_win = OpenModeWin ::BINARY,
                OpenShareFlag sh_flag = OpenShareFlag::DENY_WR) {
#if _WIN32
  auto wstr_fn = msw::encoding::utf8_to_utf16(fn);
  int returns;
  auto some_err = _wsopen_s(&returns, wstr_fn.data(), oflag | static_cast<int>(om_win), static_cast<int>(sh_flag), pmode);
  return std::make_pair(returns, some_err);
#else
  auto opened = open(fn, oflag, pmode);
  return std::make_pair(opened, errno);
#endif
}