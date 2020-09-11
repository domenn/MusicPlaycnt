#include "foo_np_log_parser.hpp"

#include <algorithm>
#include <src/misc/consts.hpp>
#include <src/misc/utilities.hpp>
#include <src/misc/utiltime.hpp>
#include <src/model/app_config.hpp>
#include <src/win/winapi_exceptions.hpp>

using namespace std::string_literals;

msw::model::SongWithMetadata replace_action_type(msw::model::SongWithMetadata&& input,
                                                 msw::model::ActionType action_type) {
  input.set_action_type(action_type);
  return std::move(input);
}

msw::model::SongWithMetadata msw::musicstuffs::FooNpLogParser::produce() {
  msw::model::SongWithMetadata song_with_metadata;
  song_with_metadata.set_timestamp_of_action(msw::helpers::parse_from_string(timestamp_, consts::FOO_TIME_FMT));
  song_with_metadata.set_action_type(model::from_foo_string(status_));
  if (song_with_metadata.action_type() == model::ActionType::STOP) {
    return replace_action_type(operator model::SongWithMetadata(), model::ActionType::STOP);
  }
  song_with_metadata.get_song().set_artist(std::move(song_items_.artist));
  song_with_metadata.get_song().set_album(std::move(song_items_.album));
  song_with_metadata.get_song().set_title(std::move(song_items_.title));
  song_with_metadata.get_song().set_path(std::move(song_items_.path));
  return song_with_metadata;
}

msw::musicstuffs::FooNpLogParser::FooNpLogParser(const model::AppConfig& app_config) : app_config_(app_config) {}

msw::musicstuffs::FooNpLogParser::operator msw::model::SongWithMetadata() {
  try {
    if (!reverse_line_reader_.has_value()) {
      init_lines_reader();
    }
    auto line = reverse_line_reader_->next();
    helpers::Utilities::trim(line);
    // A little unsafe. write_ptr iterates my variables, so they must not be reordered and things like that.
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
    song_items_.path = line.substr(idx_delim_end);

    return produce();
  } catch (const std::exception& x) {
    throw msw::exceptions::ApplicationError(
        ("Error parsing line from file\n  "s + typeid(x).name() + '-' + x.what()).c_str(), MSW_TRACE_ENTRY_CREATE);
  }
}
