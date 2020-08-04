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
    size_t idx_delim_start;
    size_t idx_delim_end = 0;
    for (const std::string& delim : app_config_.iterate_delimiters()) {
      idx_delim_start = line.find(delim, idx_delim_end);
      write_ptr->operator=(line.substr(idx_delim_end, idx_delim_start - idx_delim_end));
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

//template <typename iterator_t>
//std::string handle_no_newline_at_end(std::vector<char>& cs, iterator_t& reverse_iterator) {
//  
//}

std::string msw::musicstuffs::FooNpLogParser::extract_last_line() const {
  constexpr int LINE_SIZE_STEP = 150;
  constexpr int LINES_TO_READ = 2;
  std::ifstream source(app_config_.file_to_listen(), std::ios::binary);
  source.seekg(0, std::ios_base::end);
  size_t size = static_cast<size_t>(source.tellg());
  std::vector<char> buffer;
  int newlineCount = 0;
  while (source
         && buffer.size() != size
         && newlineCount < LINES_TO_READ) {
    buffer.resize(std::min(buffer.size() + LINE_SIZE_STEP, size));
    source.seekg(-static_cast<std::streamoff>(buffer.size()),
                 std::ios_base::end);
    source.read(buffer.data(), buffer.size());
    newlineCount = std::count(buffer.begin(), buffer.end(), '\n');
  }

  auto rightmost_newline = std::next(std::find(buffer.rbegin(), buffer.rend(), '\n'));
  if (std::distance(buffer.rbegin(), rightmost_newline) > 3) {
    // no newline at the end
    return {rightmost_newline.base() + 1, buffer.end()};
            }
            auto next_newline = std::find(rightmost_newline, buffer.rend(), '\n');
            return std::string(next_newline.base(), rightmost_newline.base());
            }
