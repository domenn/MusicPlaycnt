#include "foo_np_log_parser.hpp"

#include <fstream>
#include <vector>
#include <algorithm>
#include "src/misc/utilities.hpp"
#include "src/model/app_config.hpp"
#include "src/win/winapi_exceptions.hpp"
#include "src/misc/utiltime.hpp"
#include "src/misc/consts.hpp"

using namespace std::string_literals;

msw::model::SongWithMetadata msw::musicstuffs::FooNpLogParser::produce() {
  msw::model::SongWithMetadata song_with_metadata;
  song_with_metadata.set_timestamp_of_action(
      msw::helpers::parse_from_string(timestamp_, consts::FOO_TIME_FMT));
  song_with_metadata.set_action_type(model::from_foo_string(status_));
  song_with_metadata.get_song().set_artist(std::move(artist_));
  song_with_metadata.get_song().set_album(std::move(album_));
  song_with_metadata.get_song().set_title(std::move(title_));
  song_with_metadata.get_song().set_path(std::move(path_));
  return song_with_metadata;
}

msw::musicstuffs::FooNpLogParser::FooNpLogParser(const model::AppConfig& app_config)
  : app_config_(app_config) {
}

msw::musicstuffs::FooNpLogParser::operator msw::model::SongWithMetadata() {
  try {
    auto line = extract_last_line();
    helpers::Utilities::trim(line);
    std::string* write_ptr = &timestamp_;
    size_t idx_delim_end = 0;
    for (const std::string& delim : app_config_.iterate_delimiters()) {
      const size_t idx_delim_start = line.find(delim, idx_delim_end);
      write_ptr->operator=(line.substr(idx_delim_end, idx_delim_start - idx_delim_end));
      if ((*write_ptr) == "?") {
        write_ptr->operator=("");
      }
      idx_delim_end = idx_delim_start + delim.size();
      ++write_ptr;
    }
    path_ = line.substr(idx_delim_end);

    return produce();
  } catch (const std::exception& x) {
    throw msw::exceptions::ApplicationError(
        ("Error parsing line from file\n  "s + typeid(x).name() + '-' + x.what()).c_str(),
        MSW_TRACE_ENTRY_CREATE);
  }
}