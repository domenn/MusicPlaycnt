#include "../win/windows_headers.hpp"

#include "encoding.hpp"

namespace msw::encoding {
std::string utf16_to_utf8(const std::wstring& utf16) {
  // Special case of empty input string
  if (utf16.empty()) {
    // Return empty string
    return {};
  }

  // "Code page" value used with WideCharToMultiByte() for UTF-8 conversion
  const UINT codePageUtf8 = CP_UTF8;

  // Safely fails if an invalid UTF-16 character is encountered
  const DWORD flags = WC_ERR_INVALID_CHARS;

  // Get the length, in chars, of the resulting UTF-8 string
  const int utf8Length =
      ::WideCharToMultiByte(codePageUtf8,                    // convert to UTF-8
                            flags,                           // conversion flags
                            utf16.c_str(),                   // source UTF-16 string
                            static_cast<int>(utf16.size()),  // length of source UTF-16 string, in WCHARs
                            nullptr,
                            // unused - no conversion required in this step
                            0,
                            // request size of destination buffer, in chars
                            nullptr,
                            nullptr) +
      1;
  if (utf8Length == 1) {
    // Conversion error
    return {};
  }

  // Allocate destination buffer to store the resulting UTF-8 string
  char* bytes = new char[utf8Length]{0};

  // Do the conversion from UTF-16 to UTF-8
  int result = ::WideCharToMultiByte(codePageUtf8,                    // convert to UTF-8
                                     flags,                           // conversion flags
                                     utf16.c_str(),                   // source UTF-16 string
                                     static_cast<int>(utf16.size()),  // length of source UTF-16 string, in WCHARs
                                     bytes,                           // pointer to destination buffer
                                     utf8Length,                      // size of destination buffer, in chars
                                     nullptr,
                                     nullptr);  // unused
  if (result == 0) {
    return {};
  }

  auto returning = std::string(bytes);
  delete[] bytes;
  // Return resulting UTF-8 string
  return returning;
}

std::wstring utf8_to_utf16(const char* const utf8) {
  int wchars_num = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
  auto* wstr = new wchar_t[wchars_num];
  MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, wchars_num);
  std::wstring result(wstr);
  delete[] wstr;
  return result;
}

}  // namespace msw::encoding
