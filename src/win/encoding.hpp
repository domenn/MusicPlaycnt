#pragma once
#include <string>

namespace msw::encoding {
std::string utf16_to_utf8(const std::wstring& utf16);
std::wstring utf8_to_utf16(const char* const utf8);
}
