#pragma once
#include <string>

namespace msw::encoding {
std::string utf16_to_utf8(const std::wstring& utf16);
std::wstring utf8_to_utf16(const char* const utf8);

template <typename char_t>
std::string ensure_utf8(std::basic_string<char_t> input) {
  if constexpr (std::is_same<char_t, wchar_t>::value) {
    return utf16_to_utf8(input);
  } else {
    return input;
  }
}

}  // namespace msw::encoding
