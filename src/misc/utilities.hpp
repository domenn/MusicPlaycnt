#pragma once
#include <src/win/windows_headers.hpp>

#include <external/cxxopt.hpp>
#include <string>
#include <vector>

namespace msw::helpers {
struct ParseSongItems;

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

  template <typename char_t>
  static bool isspace_simple(char_t ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\v' || ch == '\f' || ch == '\r';
  }

  // static std::string& ltrim(std::string& s) {
  //  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
  //  return s;
  //}

  // static std::string& rtrim(std::string& s) {
  //  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
  //  return s;
  //}

  // static std::string& trim(std::string& s) {
  //  ltrim(s);
  //  rtrim(s);
  //  return s;
  //}

  // static std::string trim(std::string&& s) {
  //  ltrim(s);
  //  rtrim(s);
  //  return std::move(s);
  //}

  static std::string& u8ltrim(std::string& s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), [](std::string::value_type ch) { return !isspace_simple(ch); }));
    return s;
  }

  static std::string& u8rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](std::string::value_type ch) { return !isspace_simple(ch); }).base(),
            s.end());
    return s;
  }

  static std::string& u8trim(std::string& s) {
    u8ltrim(s);
    u8rtrim(s);
    return s;
  }

  static std::string u8trim(std::string&& s) {
    u8ltrim(s);
    u8rtrim(s);
    return std::move(s);
  }

  /**
   * \brief "Optimized" version that does not call strlen.
   *
   * \tparam char_type char or wchar_t mostly.
   * \param[in] input input to search within.
   * \param[in] starting What to find.
   * \param[in] match_size Number of characters in `to_find` to try to match.
   * \return true if `input` starts with `starting`.
   */
  template <typename char_type = std::string::value_type>
  static bool starts_with(const std::basic_string<char_type>& input,
                          const char_type* starting,
                          size_t const match_size) {
    return !input.compare(0, match_size, starting, match_size);
  }

  static std::string& erase_all_of(std::string& str, char character);
  static std::string& transform_replace_all(std::string& str, char from, const char* to, size_t to_size);

  template <typename char_type>
  static bool is_empty_or_spaces(std::basic_string<char_type> string);
  static std::string shorten_path(const std::string& long_absolute_path);
};

class CmdParse {
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
  [[nodiscard]] std::string import_legacy_path() const;
  [[nodiscard]] bool help() const;
};
}  // namespace msw::helpers

template <typename char_type>
bool msw::helpers::Utilities::is_empty_or_spaces(std::basic_string<char_type> string) {
  return string.find_first_not_of(' ') == std::basic_string<char_type>::npos;
}