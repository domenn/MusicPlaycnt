#pragma once
#include <string>

namespace msw {
namespace model {
class Song;
}
class StringProvider {
  int seq_{0};
  std::string default_prefix_{};

 public:
  StringProvider() = default;
  StringProvider(std::string default_prefix) : default_prefix_(std::move(default_prefix)) {}

  std::string get_str(std::string&& prefix = "");

  std::tuple<std::string, std::string, std::string, std::string> four_strings(std::string&& prefix = "");

  static msw::model::Song song_with_playcnt(
      std::string a, std::string b, std::string c, std::string d, int32_t playcnt);
  msw::model::Song get_simple_song();
};

/**
 * Make path (from token)
 *
 * \param token final part of path ... for example "filename.mp3"
 * \return absolute / relative path, according to config and with conformance with expectation. Example:
 * "C:\whatever\filename.mp3"
 */
std::string p_mk(std::string&& token);

}  // namespace msw
