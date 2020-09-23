#pragma once
#include <string>

namespace msw {
namespace model {
class Song;
}
class StringProvider {
  int seq = 0;

 public:
  std::string get_str(std::string&& prefix = "");

  std::tuple<std::string, std::string, std::string, std::string> four_strings(std::string&& prefix = "");

  static msw::model::Song song_with_playcnt(std::string a, std::string b, std::string c, std::string d, int32_t playcnt);
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
