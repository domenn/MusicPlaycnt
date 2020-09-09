#pragma once
#include <string>
#include <fstream>
#include "src/model/song_with_metadata.hpp"

namespace msw {
namespace model {
class AppConfig;
}
}

namespace msw::musicstuffs {

class FooNpLogParser {
  class LineGetter {
    constexpr static int LINE_SIZE_STEP = 150;
    std::ifstream ifstream_;
    std::vector<char> buffer_;
    // How many of them are in buffer? Note that there is one at the end of the file.
    int number_of_newlines_{0};
    // it is actually reverse. Tells me number of newline where the line that I read the last starts.
    // 0 means last newline, at the end. 1 means last line (second to last newline) and so on.
    int number_of_last_read_line_{0};
    size_t idx_in_buffer_of_relevant_newline_;
    size_t file_size_;

    void load_part_of_file();

  public:
    std::string next();
    LineGetter(std::ifstream ifstream1);
  };

  const model::AppConfig& app_config_;

  std::string timestamp_{};
  std::string status_{};
  helpers::ParseSongItems song_items_{};

  msw::model::SongWithMetadata produce();

public:

  FooNpLogParser(const model::AppConfig& app_config);

  explicit operator msw::model::SongWithMetadata();
  LineGetter lines() const;


};
} // namespace msw
