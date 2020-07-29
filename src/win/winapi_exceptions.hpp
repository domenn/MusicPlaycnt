#pragma once
#include <string>
#include <stdexcept>

#include <winerror.h>

namespace msw::exceptions {

struct TraceEntry {
  int line_{};
  std::string function_{};
  std::string filename_{};
};

class ApplicationError : public std::runtime_error {
protected:
  const int line_{};
  const std::string function_{};
  const std::string filename_{};

  mutable std::string cached_what_{};

public:
  ApplicationError(const char* message, TraceEntry trace_entry)
    : runtime_error(message),
      line_(std::move(trace_entry.line_)),
      function_(std::move(trace_entry.function_)),
      filename_(std::move(trace_entry.filename_)) {
  }
  char const* what() const override;
  virtual ~ApplicationError() = default;
};

class WinApiError : public ApplicationError {
protected:
  const int error_code_;
  std::string getlasterror_function_;

public:
  std::wstring what_w() const;

  WinApiError(int error_code,
              std::string function,
              TraceEntry trace_entry,
              const char* user_message = "")
    : ApplicationError(user_message, std::move(trace_entry)),
      error_code_(error_code),
      getlasterror_function_(std::move(function)) {
  }

  char const* what() const override;
  virtual std::wstring win_error_message() const;
};

} // namespace printreg::exceptions


#define MAKE_DIAG_WIN_API_ERR(code, func) \
    msw::exceptions::WinApiError(code, func, {__LINE__, __FUNCTION__, __FILE__})
