#pragma once
#include <external/cxxopt.hpp>
#include <src/win/windows_headers.hpp>
#include <string>
#include <vector>

namespace msw::helpers {
class ParseSongItems;

class Utilities {
 public:
  static std::string app_folder();
  static std::string file_in_app_folder(std::string file_name);
  static std::string file_in_app_folder_with_creating_app_folder(std::string file_name);
  static std::string get_parent_folder(const std::string& path);
  template <typename char_t>
  static std::pair<std::basic_string<char_t>, std::basic_string<char_t>> get_parent_folder_and_filename(
      const std::basic_string<char_t>& cs);

  static void start_non_executable_file(const std::string& path);

  static std::string get_thread_description();

  static std::string& ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
    return s;
  }

  static std::string& rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
  }

  static std::string& trim(std::string& s) {
    ltrim(s);
    rtrim(s);
    return s;
  }
};

class CmdParse {
  static constexpr char BAD_VAL[] = " _-1BbB0q0Qa!qQQQ";

  class ArgcArgv {
   public:
    const int argc_;
    const char** argv_;
#ifdef _WIN32
    const std::vector<char> raw_data_{};
    const std::vector<const char*> pointers_{};
#endif
    static ArgcArgv from_wstring(const wchar_t* wins_cmdline);
  };

  const ArgcArgv argc_argv_;
  cxxopts::Options options_;
  const cxxopts::ParseResult result_;

  [[nodiscard]] static cxxopts::Options create_options();

  static void try_get_throw(const std::string& parameter_name,
                            const char* const as_name,
                            const std::exception& thrown_x);

  template <typename T>
  T try_get(const std::string& parameter_name) const {
    try {
      return result_[parameter_name].as<T>();
    } catch (std::exception& x) {
      try_get_throw(parameter_name, typeid(T).name(), x);
      return {};
    }
  }

 public:
  CmdParse(const int argc, const char** argv);
#ifdef _WIN32
  CmdParse(const wchar_t* lpCmdLine);
#endif
  [[nodiscard]] bool is_listen() const;
  [[nodiscard]] ParseSongItems song_data() const;
};
}  // namespace msw::helpers
