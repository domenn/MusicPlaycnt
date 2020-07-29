// ReSharper disable once CppUnusedIncludeDirective
#include <src/win/windows_headers.hpp>

#include "winapi_exceptions.hpp"
#include "windows_error_format.hpp"
#include "encoding.hpp"

using namespace std::literals::string_literals;

char const* msw::exceptions::ApplicationError::what() const {
  const auto parent_what = runtime_error::what();
  cached_what_.clear();
  if (parent_what != nullptr && *parent_what != '\0') {
    cached_what_ += parent_what + "\n "s;
  }
  if (!function_.empty() || !filename_.empty() || line_) {
    cached_what_ += "At: "s + function_ + "(" + filename_ + ":" + std::to_string(line_) + ")";
  }
  return cached_what_.c_str();
}

std::wstring msw::exceptions::WinApiError::what_w() const {
  return msw::encoding::utf8_to_utf16(what());
}

char const* msw::exceptions::WinApiError::what() const {
  // ReSharper disable once CppExpressionWithoutSideEffects
  ApplicationError::what();
  cached_what_ += encoding::utf16_to_utf8(win_error_message());
  return cached_what_.c_str();
}

std::wstring msw::exceptions::WinApiError::win_error_message() const {
  return msw::windows::format_windows_error_w(error_code_,
                                              encoding::utf8_to_utf16(getlasterror_function_.c_str()).c_str());
}
