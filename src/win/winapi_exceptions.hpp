#pragma once
#include <external/StackWalker.h>

#include <ostream>
#include <src/misc/custom_include_spdlog.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace msw::exceptions {

struct TraceEntry {
  int line_{};
  std::string function_{};
  std::string filename_{};

  friend std::ostream& operator<<(std::ostream& os, const TraceEntry& obj) {
    return os << obj.filename_ << '(' << obj.line_ << "): " << obj.function_;
  }
};

#ifdef _MSC_VER
class AppStackWalker : public StackWalker {
  std::vector<TraceEntry> stack_trace_{};

 public:
  AppStackWalker() : StackWalker() {
    ShowCallstack();
    stack_trace_.erase(stack_trace_.begin());
  }

  operator std::vector<TraceEntry>() { return std::move(stack_trace_); }

  friend std::ostream& operator<<(std::ostream& os, const AppStackWalker& obj) {
    if (obj.stack_trace_.empty()) {
      return os;
    }

    std::for_each(
        obj.stack_trace_.begin(), obj.stack_trace_.begin() + 1, [&os](const TraceEntry& itm) { os << "  " << itm; });
    std::for_each(
        obj.stack_trace_.begin() + 1, obj.stack_trace_.end(), [&os](const TraceEntry& itm) { os << "\n  " << itm; });
    return os;
  }

  /**
   * Intended for testing. `custom_value` items are written into filename_ of stacktrace vector.
   *
   * \param custom_value to set. Old contents get overwritten.
   */
  void set_custom_value(std::vector<std::string>&& custom_value) {
    // part 1 - replace existing items
    const auto transformer = [](std::string&& copying_thing) { return TraceEntry{0, {}, std::move(copying_thing)}; };

    const auto part1_iterator_right = custom_value.begin() + (std::min)(custom_value.size(), stack_trace_.size());
    const auto written_location = std::transform(std::make_move_iterator(custom_value.begin()),
                                                 std::make_move_iterator(part1_iterator_right),
                                                 stack_trace_.begin(),
                                                 transformer);

    if (part1_iterator_right == custom_value.end()) {
      // new length is shorter than old
      stack_trace_.erase(written_location, stack_trace_.end());
    } else {
      // new length is longer - some items still need copying.
      std::transform(std::make_move_iterator(part1_iterator_right),
                     std::make_move_iterator(custom_value.end()),
                     std::back_inserter(stack_trace_),
                     transformer);
    }
  }

 protected:
  void OnCallstackEntry(CallstackEntryType eType, CallstackEntry& entry) override {
    stack_trace_.push_back({static_cast<int>(entry.lineNumber), entry.name, entry.lineFileName});
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

#define MSW_TRACE_ENTRY_CREATE \
  { __LINE__, __FUNCTION__, __FILE__ }

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
        stack_trace_(AppStackWalker()) {}

  void add_to_stack_trace(TraceEntry trace_entry) { stack_trace_.emplace_back(std::move(trace_entry)); }

  char const* what() const noexcept override;
  virtual ~ApplicationError() = default;

#ifdef _WIN32
  std::wstring what_w() const;
#endif

  std::string stacktrace() const {
    std::ostringstream oss;
    oss << " at: ";
    std::for_each(stack_trace_.begin(), stack_trace_.end(), [&oss](const TraceEntry& itm) { oss << "\n  " << itm; });
    return oss.str();
  }
};

class ErrorCode : public ApplicationError {
 protected:
  const int error_code_;
  std::string getlasterror_function_;

 public:
  ErrorCode(int error_code, std::string function, TraceEntry trace_entry, const char* user_message = "")
      : ApplicationError(user_message, std::move(trace_entry)),
        error_code_(error_code),
        getlasterror_function_(std::move(function)) {}

  char const* what() const noexcept override;
  bool is_enoent() const;
  bool is_already_exists() const;
  int code() const { return error_code_; }
};

class WinApiError : public ErrorCode {
 public:
  char const* what() const noexcept override;

  WinApiError(int error_code, std::string function, TraceEntry trace_entry, const char* user_message = "")
      : ErrorCode(error_code, std::move(function), std::move(trace_entry), user_message) {}

  virtual std::wstring win_error_message() const;
};

}  // namespace msw::exceptions

#define MAKE_DIAG_WIN_API_ERR(code, func) msw::exceptions::WinApiError(code, func, {__LINE__, __FUNCTION__, __FILE__})
