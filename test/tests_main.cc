
#include <src/win/windows_headers.hpp>

#include "tests_main.hpp"

#include "persistence_test.hpp"
#include <sago/platform_folders.h>
#include <src/data/accessor.hpp>
#include <src/misc/consts.hpp>
#include <src/misc/utilities.hpp>
#include <src/model/song_list.hpp>
#include <src/model/song_with_metadata.hpp>
#include <test/util/tests_accessor.hpp>

using string = std::string;

int main(int argc, char** argv) {
  spdl::spdlog_setup(spdl::SpdlogConfig::build()
                         .default_logger_name("Music2_test")
                         .file_name("NewMusicTrackerCounter.log")
                         .log_to_file(false)
                         .level(spdlog::level::debug)
                         .pattern(spdl::SpdlogConfig::PATTERN_ALL_DATA));

  // msw::data::Accessor<msw::model::SongList> inst_song_list;
  msw::data::TestsAccessor<msw::model::SongWithMetadata> inst_handled_song;
  // msw::pg::song_list = &inst_song_list;
  msw::pg::handled_song = &inst_handled_song;

  msw::model::AppConfig tests_config;
  tests_config.set_my_defaults();
  msw::pg::app_config = &tests_config;

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

std::string msw::helpers::Utilities::app_folder() { return sago::getConfigHome() + '/' + consts::TESTS_DIR; }

std::string msw::StringProvider::get_str(std::string&& prefix) {
  string right_part = std::to_string(seq++);
  return std::move(prefix) + "_testStr__" + right_part + right_part + right_part + ".one";
}

std::tuple<std::string, std::string, std::string, std::string> msw::StringProvider::four_strings(std::string&& prefix) {
  return std::make_tuple(get_str(std::string(prefix)),
                         get_str(std::string(prefix)),
                         get_str(std::string(prefix)),
                         get_str(std::move(prefix)));
}

void msw::musicstuffs::FooNpLogParser::init_lines_reader() {
  const auto* downcasted_me = reinterpret_cast<MockFooNpLogParser*>(this);
  reverse_line_reader_.emplace(std::make_unique<std::istringstream>(downcasted_me->line_to_parse_));
}
