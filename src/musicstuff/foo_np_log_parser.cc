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
  song_with_metadata.get_song().set_artist(std::move(song_items_.artist));
  song_with_metadata.get_song().set_album(std::move(song_items_.album));
  song_with_metadata.get_song().set_title(std::move(song_items_.title));
  song_with_metadata.get_song().set_path(std::move(song_items_.path));
  return song_with_metadata;
}

msw::musicstuffs::FooNpLogParser::FooNpLogParser(const model::AppConfig& app_config)
  : app_config_(app_config) {
}

msw::musicstuffs::FooNpLogParser::operator msw::model::SongWithMetadata() {
  try {
    auto lines_inst = lines();
    auto line = lines_inst.next();
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
        ("Error parsing line from file\n  "s + typeid(x).name() + '-' + x.what()).c_str(),
        MSW_TRACE_ENTRY_CREATE);
  }
}

void msw::musicstuffs::FooNpLogParser::LineGetter::load_part_of_file() {
  size_t size = static_cast<size_t>(ifstream_.tellg());

  ifstream_.seekg(-static_cast<std::streamoff>(LINE_SIZE_STEP), std::ios_base::cur);
  auto tellg = ifstream_.tellg();

    ifstream_.read(buffer_.data(), LINE_SIZE_STEP);


  auto rightmost_newline = std::next(std::find(buffer_.rbegin(), buffer_.rend(), '\n'));
  if (std::distance(buffer_.rbegin(), rightmost_newline) > 3) {
    // no newline at the end
    return {rightmost_newline.base() + 1, buffer_.end()};
  }
  auto next_newline = std::find(rightmost_newline, buffer_.rend(), '\n');
  return std::string(next_newline.base(), rightmost_newline.base());
}