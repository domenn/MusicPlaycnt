#pragma once
#include <string>
#include "src/model/song_with_metadata.hpp"

namespace msw {
namespace model {
class AppConfig;
}
}

namespace msw::musicstuffs {

class FooNpLogParser {
  const model::AppConfig& app_config_;

  std::string timestamp_{};
  std::string status_{};
  std::string artist_{};
  std::string album_{};
  std::string title_{};
  std::string path_{};

  msw::model::SongWithMetadata produce();

public:

  FooNpLogParser(const model::AppConfig& app_config);

  explicit operator msw::model::SongWithMetadata();
  std::string extract_last_line() const;


};
} // namespace msw
