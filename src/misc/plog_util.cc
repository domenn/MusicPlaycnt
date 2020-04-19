#include "plog_util.hpp"

#include <plog/Appenders/ColorConsoleAppender.h>
#include <sago/platform_folders.h>
#include <filesystem>

std::string PlogUtil::make_full_fn_appdata(const std::string& app_name, const std::string& log_fn) {
  return make_full_fn_appdata(app_name + "/" + log_fn);
}

std::string PlogUtil::make_full_fn_appdata(const std::string& log_fn) {
  const auto str_fn = sago::getDataHome() + "/" + log_fn;
  std::filesystem::path fspath_fn(str_fn);
  try {
    create_directories(fspath_fn.parent_path());
  } catch (const std::filesystem::filesystem_error& ignored_err) {

  }
  return str_fn;
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos) return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

const char* CustomTextFormater::sev_tostring_custom(plog::Severity sev) {
  if (sev == plog::Severity::warning) {
    return " warning ";
  }
  if (sev <= plog::Severity::error) {
    return " error ";
  }
  return "";
}

plog::util::nstring CustomTextFormater::filename_rightmost(const char* const fn_full, bool override_passtrough) {
  const auto fn_str = std::string(fn_full);
  const auto idx = fn_str.find_last_of("/\\") + 1;

  // const std::string thing_to_write = (override_passtrough ? fn_str : fn_str.substr(idx));

  // Experimenting ...
  // std::string thing_to_write = fn_str.substr(idx);
  /*

In addition, adding " : error" or ": warning" to the end makes Visual Studio color it red or yellow
https://stackoverflow.com/questions/12301055/double-click-to-go-to-source-in-output-window/12369186#12369186

Basically this is C# I think ... c++ doesn't really work well here.


   */
  auto thing_to_write = fn_str;
  replace(thing_to_write, "C:\\Users\\Domen\\CLionProjects\\musicPlaycnt", "..");

  if constexpr (
    std::is_same<plog::util::nstring::value_type, wchar_t>::value) {
    return plog::util::toWide(thing_to_write.c_str());
  } else if constexpr (std::is_same<plog::util::nstring::value_type, char>::value) {
#ifndef _WIN32
    return (plog::util::nstring)fn_str.substr(idx);
#endif
  }
}

plog::util::nstring CustomTextFormater::header()
// This method returns a header for a new file. In our case it is empty.
{
  return plog::util::nstring();
}

plog::util::nstring CustomTextFormater::format(const plog::Record& record)
// This method returns a string from a record.
//{
//  constexpr bool useUtcTime = false;
//  tm t;
//  (useUtcTime ? plog::util::gmtime_s : plog::util::localtime_s)(&t, &record.getTime().time);
//
//  plog::util::nostringstream ss;
//  ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-")
//      << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
//  ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0'))
//      << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec
//      << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << PLOG_NSTR(" ");
//  ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << severityToString(record.getSeverity())
//      << PLOG_NSTR(" ");
//  ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
//
//  ss << record.getFunc() << PLOG_NSTR("(") <<
//      filename_rightmost(record.getFile())
//      <<
//      PLOG_NSTR(":") << record.getLine() << PLOG_NSTR(") - ");
//  ss << record.getMessage() << PLOG_NSTR("\n");
//
//  return ss.str();
//}


{
  constexpr bool useUtcTime = false;
  tm t;
  (useUtcTime ? plog::util::gmtime_s : plog::util::localtime_s)(&t, &record.getTime().time);

  plog::util::nostringstream ss;
  //ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-")
  //    << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
  //ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0'))
  //    << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec
  //    << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << PLOG_NSTR(" ");
  //ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << severityToString(record.getSeverity())
  //    << PLOG_NSTR(" ");
  //ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ") << record.getFunc() << PLOG_NSTR(" ");
  ss << filename_rightmost(record.getFile()) << PLOG_NSTR("(") << record.getLine()
      << PLOG_NSTR(")")
      //<< (record.getSeverity() >=
      //    plog::Severity::warning
      //      ? severityToString(record.getSeverity())
      //      : "")

      << sev_tostring_custom(record.getSeverity())
      << PLOG_NSTR(": ");
  ss << record.getMessage() << PLOG_NSTR("\n");

  return ss.str();
}
