#pragma once
#include <string>
#include <stdexcept>

#include <winerror.h>
#include <vector>
#include <external/StackWalker.h>
#include <src/misc/custom_include_spdlog.hpp>
#include <sstream>
#include <ostream>

namespace msw::exceptions {

struct TraceEntry {
  int line_{};
  std::string function_{};
  std::string filename_{};


  friend std::ostream& operator<<(std::ostream& os, const TraceEntry& obj) {
    return os
           << obj.filename_ << '(' << obj.line_ << "): " << obj.function_;
  }
};

#ifdef _MSC_VER
class AppStackWalker : public StackWalker {
  std::vector<TraceEntry> stack_trace_{};
public:
  AppStackWalker()
    : StackWalker() {
    ShowCallstack();
    stack_trace_.erase(stack_trace_.begin());
  }

  operator std::vector<TraceEntry>() {
    return std::move(stack_trace_);
  }

protected:
  void OnCallstackEntry(CallstackEntryType eType, CallstackEntry& entry) override {
    stack_trace_.push_back({static_cast<int>(entry.lineNumber),
                            entry.name,
                            entry.lineFileName});
  }
};
#else
class AppStackWalker {
public:
  operator std::vector<TraceEntry>() {
    // not implemented
    return {};
  }
};
#endif

#define MSW_TRACE_ENTRY_CREATE {__LINE__, __FUNCTION__, __FILE__}

class ApplicationError : public std::runtime_error {
protected:
  const int line_{};
  const std::string function_{};
  const std::string filename_{};

  mutable std::string cached_what_{};

  std::vector<TraceEntry> stack_trace_{};

public:
  ApplicationError(const char* message, TraceEntry trace_entry)
    : runtime_error(message),
      line_(std::move(trace_entry.line_)),
      function_(std::move(trace_entry.function_)),
      filename_(std::move(trace_entry.filename_)),
      stack_trace_(AppStackWalker()) {
  }

  void add_to_stack_trace(TraceEntry trace_entry) {
    stack_trace_.emplace_back(std::move(trace_entry));
  }

  char const* what() const noexcept override;
  virtual ~ApplicationError() = default;

  std::string stacktrace() const {
    std::ostringstream oss;
    oss << " at: ";
    std::for_each(stack_trace_.begin(),
                  stack_trace_.end(),
                  [&oss](const TraceEntry& itm)
                  {
                    oss << "\n  " << itm;
                  });
    return oss.str();
  }
};

class ErrorCode : public ApplicationError {
protected:
  const int error_code_;
  std::string getlasterror_function_;

public:
  ErrorCode(int error_code,
            std::string function,
            TraceEntry trace_entry,
            const char* user_message = "")
    : ApplicationError(user_message, std::move(trace_entry)),
      error_code_(error_code),
      getlasterror_function_(std::move(function)) {
  }

  char const* what() const noexcept override;
  bool is_enoent() const;
  bool is_already_exists() const;
  int code() const { return error_code_; }
};

class WinApiError : public ErrorCode {
public:
  std::wstring what_w() const;
  char const* what() const noexcept override;

  WinApiError(int error_code,
              std::string function,
              TraceEntry trace_entry,
              const char* user_message = "")
    : ErrorCode(error_code, std::move(function), std::move(trace_entry), user_message) {
  }

  virtual std::wstring win_error_message() const;
};

} // namespace printreg::exceptions


#define MAKE_DIAG_WIN_API_ERR(code, func) \
    msw::exceptions::WinApiError(code, func, {__LINE__, __FUNCTION__, __FILE__})
