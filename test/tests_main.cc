
#include <src/win/windows_headers.hpp>

#include "tests_main.hpp"

#include "flow_tests.hpp"
#include "persistence_test.hpp"
#include "util/tests_util_functions.hpp"
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
                         .level(spdlog::level::trace)
                         .pattern(spdl::SpdlogConfig::PATTERN_ALL_DATA));

  msw::data::TestsAccessor<msw::model::SongList> inst_song_list;
  msw::data::TestsAccessor<msw::model::SongWithMetadata> inst_handled_song;
  msw::pg::song_list = &inst_song_list;
  msw::pg::handled_song = &inst_handled_song;

  msw::model::AppConfig tests_config;
  tests_config.set_my_defaults();
  msw::pg::app_config = &tests_config;

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

std::string msw::helpers::Utilities::app_folder() { return sago::getConfigHome() + '/' + consts::TESTS_DIR; }

std::string msw::StringProvider::get_str(std::string&& prefix) {
  string right_part = std::to_string(seq_++);
  return default_prefix_ + std::move(prefix) + "_testStr__" + right_part + right_part + right_part + ".one";
}

std::tuple<std::string, std::string, std::string, std::string> msw::StringProvider::four_strings(std::string&& prefix) {
  return std::make_tuple(get_str(std::string(prefix)),
                         get_str(std::string(prefix)),
                         get_str(std::string(prefix)),
                         get_str(std::move(prefix)));
}

std::string msw::p_mk(std::string&& token) {
  return (std::filesystem::path(msw::pg::app_config->library_path()) / token).generic_string();
}

void msw::musicstuffs::FooNpLogParser::init_lines_reader() {
  const auto* downcasted_me = reinterpret_cast<MockFooNpLogParser*>(this);
  reverse_line_reader_.emplace(std::make_unique<std::istringstream>(downcasted_me->line_to_parse_));
}

msw::model::Song msw::StringProvider::song_with_playcnt(
    std::string a, std::string b, std::string c, std::string d, int32_t playcnt) {
  msw::model::Song making_song(a, b, c, d);
  making_song.set_playcount(playcnt);
  return making_song;
}

msw::model::Song msw::StringProvider::get_simple_song() {
  return std::make_from_tuple<msw::model::Song>(four_strings());
}

// --gtest_catch_exceptions=0 --gtest_filter=SongModel.diff*
