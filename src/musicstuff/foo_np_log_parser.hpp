#pragma once
#include <optional>
#include <src/misc/read_line_reverse.hpp>
#include <src/model/song_with_metadata.hpp>
#include <string>

namespace msw {
namespace model {
class AppConfig;
}
}  // namespace msw

namespace msw::musicstuffs {

class FooNpLogParser {
  std::string timestamp_{};
  std::string status_{};
  helpers::ParseSongItems song_items_{};
  std::optional<ReadLineReverse> reverse_line_reader_{};

  msw::model::SongWithMetadata produce();
  void init_lines_reader();

 public:
  FooNpLogParser();

  explicit operator msw::model::SongWithMetadata();
};
}  // namespace msw::musicstuffs
