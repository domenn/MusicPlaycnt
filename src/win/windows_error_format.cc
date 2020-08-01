#ifdef _WIN32

#include <src/win/windows_headers.hpp>
#include "windows_error_format.hpp"
#include <src/win/encoding.hpp>
#include <strsafe.h>


class DeleteOnDestruct {
  LPVOID ptr_;

public:
  explicit DeleteOnDestruct(LPVOID ptr)
    : ptr_(ptr) {
  }

  ~DeleteOnDestruct() { LocalFree(ptr_); }
};

std::wstring msw::windows::format_windows_error_w(uint32_t error_code,
                                                  const wchar_t* lpszFunction) {
  LPVOID lpMsgBuf;
  FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      error_code,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      reinterpret_cast<LPTSTR>(&lpMsgBuf),
      0,
      NULL);

  const auto lpDisplayBuf = static_cast<LPVOID>(
    LocalAlloc(
        LMEM_ZEROINIT,
        (lstrlen(static_cast<LPCTSTR>(lpMsgBuf)) +
         static_cast<SIZE_T>(lstrlen(static_cast<LPCTSTR>(lpszFunction))) + 40) * sizeof(TCHAR)));
  StringCchPrintfW(static_cast<LPTSTR>(lpDisplayBuf),
                   LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                   TEXT("%s failed with error %d: %s"),
                   lpszFunction,
                   error_code,
                   lpMsgBuf);
  DeleteOnDestruct deleter0(lpMsgBuf);
  DeleteOnDestruct deleter1(lpDisplayBuf);
  return static_cast<LPTSTR>(lpDisplayBuf);
}

std::wstring msw::windows::format_get_last_error_w(const wchar_t* lpszFunction) {
  return format_windows_error_w(GetLastError(), lpszFunction);
}

std::string msw::windows::format_windows_error(uint32_t error_code, const char* function) {
  return encoding::utf16_to_utf8(format_windows_error_w(error_code, encoding::utf8_to_utf16(function).c_str()));
}


#endif  // _WIN32
