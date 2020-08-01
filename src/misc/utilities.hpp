#pragma once
#include <string>

namespace msw::helpers {
class Utilities {
public:
  static std::string app_folder();
  static std::string file_in_app_folder(std::string file_name);
  static std::string file_in_app_folder_with_creating_app_folder(std::string file_name);
  static std::string get_parent_folder(const std::string& path);
  template <typename char_t>
  static std::pair<std::basic_string<char_t>, std::basic_string<char_t>> get_parent_folder_and_filename(const std::basic_string<char_t>& cs);

  static void start_non_executable_file(const std::string&  path);

  static  std::string get_thread_description();

private:

};
}
