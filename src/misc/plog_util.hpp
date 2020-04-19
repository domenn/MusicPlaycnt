#pragma once
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Log.h>
#include <plog/Severity.h>
#ifdef _WIN32
#include <plog/Appenders/DebugOutputAppender.h>
#endif

class CustomTextFormater {
  static const char *  sev_tostring_custom(plog::Severity sev);

   public:
  static plog::util::nstring filename_rightmost(const char* const fn_full,
      bool override_passtrough = true);

  static plog::util::nstring header();
  static plog::util::nstring format(const plog::Record& record);
};

class PlogUtil {
private:
  static std::string make_full_fn_appdata(const std::string& app_name, const std::string& log_fn);
  static std::string make_full_fn_appdata(const std::string& log_fn);

public:
  /**
   * Call before doing stuff with logging.
   * @param full_filename where to save logs. Path with fn, using / as separator.
   * @param inside_appdata if true, put to appdata. If true, I recommend `full_filename` be "AppName/logFile.log".
   * @param level of main logger.
   * @param timers_diagnostics_level level of logger of this type. Currently disabled in code.
   * @param low_priority_log_level level of logger of this type. Currently disabled in code.
   */
  static void setup_logger(const std::string& full_filename = "cppapp.log",
                           const bool inside_appdata = true,
                           plog::Severity level = plog::info,
                           plog::Severity timers_diagnostics_level = plog::debug,
                           plog::Severity low_priority_log_level = plog::warning) {
    if (plog::get()) {
      return;
    }
    static plog::RollingFileAppender<CustomTextFormater> file_appender(full_filename.c_str(),
                                                                       320000,
                                                                       4); // Create the 1st appender.
    static plog::ColorConsoleAppender<CustomTextFormater> console_appender;
    // add_appeners<PLOG_DEFAULT_INSTANCE_ID>(level, &file_appender, &console_appender);

#ifdef _WIN32
    static plog::DebugOutputAppender<CustomTextFormater> win_dbg_appender;
#endif

    //    // plog::init<CustomTextFormater, PLOG_DEFAULT_INSTANCE_ID>(level, "CustomFormatter.txt")
    plog::init<PLOG_DEFAULT_INSTANCE_ID>(level, &file_appender)
        .addAppender(&console_appender)
#ifdef _WIN32
        .addAppender(&win_dbg_appender)
#endif
        ;

    PLOG_INFO << "all the stuff set up.";

    // add_appeners<ALWAYS_INFO_LOGGER>(plog::info, &file_appender, &console_appender);

    //    if (timers_diagnostics_level > plog::none) {
    //      add_appeners<TIMING_DIAGNOSTICS_LOGGER>(timers_diagnostics_level, &file_appender,
    //                                              &console_appender);
    //    }
    //    if (low_priority_log_level > plog::none) {
    //      add_appeners<LOW_PRIORITY_LOGGER>(low_priority_log_level, &file_appender,
    //                                        &console_appender);
    //    }
  }
};
