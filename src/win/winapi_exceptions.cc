// ReSharper disable once CppUnusedIncludeDirective
#include <src/win/windows_headers.hpp>

#include "winapi_exceptions.hpp"

#include "encoding.hpp"
#include "windows_error_format.hpp"

using namespace std::literals::string_literals;

char const* msw::exceptions::InformationalApplicationError::what() const noexcept {
  try {
    const auto parent_what = runtime_error::what();
    if (parent_what != nullptr && *parent_what != '\0') {
      cached_what_ = parent_what;
      cached_what_ += '\n';
    }
    if (!function_.empty() || !filename_.empty() || line_) {
      cached_what_ += " At: "s + function_ + "(" + filename_ + ":" + std::to_string(line_) + ")";
      if (!stack_trace_.empty()) {
        cached_what_ += ' ' + stacktrace();
      }
    }
  } catch (std::bad_alloc& ba) {
    cached_what_ = "Bad_alloc was thrown during the call of what - "s + ba.what();
  }
  return cached_what_.c_str();
}

#ifdef _WIN32
std::wstring msw::exceptions::ApplicationError::what_w() const { return encoding::utf8_to_utf16(what()); }
#endif

char const* msw::exceptions::ErrorCode::what() const noexcept {
  // ReSharper disable once CppExpressionWithoutSideEffects
  InformationalApplicationError::what();
  cached_what_ += "\n " + getlasterror_function_ + " failed with code: " + std::to_string(error_code_);
  return cached_what_.c_str();
}

bool msw::exceptions::ErrorCode::is_enoent() const { return error_code_ == ENOENT; }

bool msw::exceptions::ErrorCode::is_already_exists() const { return error_code_ == EEXIST; }

char const* msw::exceptions::WinApiError::what() const noexcept {
  // ReSharper disable once CppExpressionWithoutSideEffects
  InformationalApplicationError::what();
  cached_what_ += '\n' + encoding::utf16_to_utf8(win_error_message());
  return cached_what_.c_str();
}

std::wstring msw::exceptions::WinApiError::win_error_message() const {
  return msw::windows::format_windows_error_w(error_code_,
                                              encoding::utf8_to_utf16(getlasterror_function_.c_str()).c_str());
}
