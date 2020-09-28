#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#if defined(__unix__)
#define FUNC_LOCALTIME(time, structure) localtime_r(time, structure)
#else
#define FUNC_LOCALTIME(time, structure) localtime_s(structure, time)
#endif

// Good article:
// https://developers.google.com/protocol-buffers/docs/reference/csharp/class/google/protobuf/well-known-types/timestamp

namespace utiltime::primitive_convs {
template <typename var_type>
var_type ms_to_s(var_type ms) {
  return static_cast<var_type>(ms) / static_cast<var_type>(1000);
}
}  // namespace utiltime::primitive_convs

namespace msw::helpers {

constexpr char D_DOT_M_DOT_Y__HMS[] = "%d.%m.%y_%H.%M.%S";

template <typename t_os = std::ostringstream, typename chrono_thing>
t_os format_some_input(const typename t_os::char_type* format, const chrono_thing& now_time) {
  auto itt = std::chrono::system_clock::to_time_t(now_time);
  t_os ss;
  std::tm stdtm{};
  FUNC_LOCALTIME(&itt, &stdtm);
  ss << std::put_time(&stdtm, format);
  return ss;
}

inline std::ostringstream format_current_time_stringstream(const char* format) {
  return format_some_input(format, std::chrono::system_clock::now());
}

template <typename t_os = std::ostringstream, typename chrono_thing>
void add_milliseconds(t_os& stream_to_change, const chrono_thing& time) {
  stream_to_change << std::setfill(static_cast<typename t_os::char_type>('0')) << std::setw(3)
                   << (std::chrono::duration_cast<std::chrono::milliseconds>(time) % 1000).count();
}

inline std::string format_current_time(const char* format) { return format_current_time_stringstream(format).str(); }

inline std::string current_time_h_m_s_ms() {
  auto now = std::chrono::system_clock::now();
  auto ss = format_some_input("%H:%M:%S", now);
  ss << '.' << std::setfill('0') << std::setw(3)
     << (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000).count();
  return ss.str();
}

inline std::string currentISO8601TimeUTC() { return format_current_time("%FT%TZ"); }

inline uint64_t parse_from_string(const std::string& str, const char* const format) {
  std::tm tm{};
  std::istringstream ss(str);
  ss >> std::get_time(&tm, format);
  return std::chrono::system_clock::from_time_t(std::mktime(&tm)).time_since_epoch() / std::chrono::milliseconds(1);
}
}  // namespace msw::helpers
#undef FUNC_LOCALTIME
