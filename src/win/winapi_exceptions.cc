// ReSharper disable once CppUnusedIncludeDirective
#include <src/win/windows_headers.hpp>

#include "winapi_exceptions.hpp"
#include "windows_error_format.hpp"
#include "encoding.hpp"

using namespace std::literals::string_literals;

char const* msw::exceptions::ApplicationError::what() const noexcept {
  const auto parent_what = runtime_error::what();
  if (parent_what != nullptr && *parent_what != '\0') {
    cached_what_ = parent_what;
  }
  if (!function_.empty() || !filename_.empty() || line_) {
    cached_what_ += " At: "s + function_ + "(" + filename_ + ":" + std::to_string(line_) + ")";
  }
  return cached_what_.c_str();
}

char const* msw::exceptions::ErrorCode::what() const noexcept {
  // ReSharper disable once CppExpressionWithoutSideEffects
  ApplicationError::what();
  cached_what_ += "\n " + getlasterror_function_ + " failed with code: " + std::to_string(error_code_);
  return cached_what_.c_str();
}

bool msw::exceptions::ErrorCode::is_enoent() const {
  return error_code_ == ENOENT;
}

bool msw::exceptions::ErrorCode::is_already_exists() const {
  return error_code_ == EEXIST;
}

std::wstring msw::exceptions::WinApiError::what_w() const {
  return msw::encoding::utf8_to_utf16(what());
}

char const* msw::exceptions::WinApiError::what() const noexcept {
  // ReSharper disable once CppExpressionWithoutSideEffects
  ErrorCode::what();
  cached_what_.insert(0, "TBD: IMPLEMENT CORRECT WIN EXCEPTION WHAT\n  ");
  cached_what_ += encoding::utf16_to_utf8(win_error_message());
  return cached_what_.c_str();
}

std::wstring msw::exceptions::WinApiError::win_error_message() const {
  return msw::windows::format_windows_error_w(error_code_,
                                              encoding::utf8_to_utf16(getlasterror_function_.c_str()).c_str());
}
