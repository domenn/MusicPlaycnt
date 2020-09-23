#pragma once

#include "util/tests_util_functions.hpp"
#include <gtest/gtest.h>
#include <src/data/accessor.hpp>
#include <src/data/pointers_to_globals.hpp>
#include <src/misc/spd_logging.hpp>
#include <src/misc/utilities.hpp>
#include <src/model/song.hpp>
#include <src/model/song_list.hpp>
#include <src/musicstuff/foo_np_log_parser.hpp>
#include <src/win/winapi_exceptions.hpp>

using namespace std::string_literals;
using Song = msw::model::Song;
using SongList = msw::model::SongList;

class MockFooNpLogParser : public msw::musicstuffs::FooNpLogParser {
 public:
  const std::string line_to_parse_;

  MockFooNpLogParser(std::string line_to_parse) : FooNpLogParser(), line_to_parse_(std::move(line_to_parse)) {}
};

class FoobarLineParse : public testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(FoobarLineParse, emptyStringsCmd) {
  const auto& param_pair = GetParam();
  MockFooNpLogParser parser(param_pair.first);
  msw::model::SongWithMetadata smd{parser};
  ASSERT_EQ(smd.get_song().album(), param_pair.second);
}

INSTANTIATE_TEST_SUITE_P(
    TestFoobarLineParse,
    FoobarLineParse,
    testing::Values(
        std::make_pair(
            "Tue Sep 08 12:19:02 2020;playing: Dragony ;; Masters Of The Multiverse ;; Evermore ;;; C:\\pathTo.mp3"s,
            "Masters Of The Multiverse"s),
        std::make_pair("Tue Sep 08 13:36:15 2020;playing: ? ;; ? ;; Evermore ;;; C:\\pathTo.mp3"s, ""s),
        std::make_pair("Tue Sep 08 13:36:15 2020;playing:  ;;  ;; Evermore ;;; C:\\pathTo.mp3"s, ""s),
        std::make_pair("Tue Sep 08 13:36:15 2020;playing:   ;;   ;; Evermore ;;; C:\\pathTo.mp3"s, " "s),
        std::make_pair(
            "Tue Sep 09 16:22:11 2020;playing: a ;; b ;; c ;;; d\nTue Sep 10 11:23:47 2020;stopped: e ;; f ;; g ;;; h"s,
            "b"s),
        std::make_pair(
            "Tue Sep 09 16:22:11 2020;playing: a ;; b ;; c ;;; d\nTue Sep 10 11:23:47 2020;paused: e ;; f ;; g ;;; h"s,
            "f"s),

        std::make_pair("Tue Sep 10 19:22:11 2020;playing: a ;; b ;; c ;;; d\nnot running"s, "b"s))

);

TEST(Fundamental, createSaveAndRestore) {
  msw_proto_song::Song proto_song;
  proto_song.set_album("oneAlbum");
  proto_song.set_artist("oneArtist");
  proto_song.set_title("TITTELLL!!!!");
  proto_song.set_path("OnePath ....");
  proto_song.set_playcnt(42);

  msw::model::Song song1(std::move(proto_song));
  std::string serd = song1.serialize();
  SPDLOG_INFO("Serialized first {}\n(as str: {})", serd, song1);

  msw::model::Song song2 = msw::model::Song::deserialize(serd);
  SPDLOG_INFO("Restored and again serd ... what happens: {}", serd);
  ASSERT_EQ(song1, song2);
}

TEST(Interface, addOneSong) {
  SongList single_sl({"1", "22", "3", "aa"});
  ASSERT_EQ(single_sl.size(), 1);
  ASSERT_EQ(single_sl[0].album(), "1");
}

TEST(Interface, addTwoSongs) { ASSERT_EQ(SongList(Song{"1", "2", "3", "4"}, Song{"5", "6", "7", "8"}).size(), 2); }

TEST(Interface, addThreeSongs) {
  msw::StringProvider sp;
  Song song1(sp.get_str(), sp.get_str(), sp.get_str(), sp.get_str());
  Song song2(sp.get_str(), sp.get_str(), sp.get_str(), sp.get_str());
  Song song3(sp.get_str(), sp.get_str(), sp.get_str(), sp.get_str());

  SongList sl(std::move(song1), std::move(song2), std::move(song3));
  ASSERT_EQ(sl.size(), 3);
}

TEST(Interface, hundredsOfSongs) {
  msw::StringProvider sp;
  spdlog::logger timeLogger("testTimeLogger");
  SPDLOG_LOGGER_DEBUG(spdl::get("HundredsSongsTiming"), "starting ...");
  auto sl = SongList(std::make_from_tuple<Song>(sp.four_strings()));
  // SongList s1(sp.get_str(), sp.get_str(), sp.get_str(), sp.get_str());
  for (int i = 0; i < 1211; ++i) {
    std::apply(&SongList::make_song_and_add, std::tuple_cat(std::make_tuple(&sl), sp.four_strings()));
  }
  SPDLOG_LOGGER_DEBUG(spdl::get("HundredsSongsTiming"), "added many");

  auto serd = sl.serialize();
  SPDLOG_LOGGER_DEBUG(spdl::get("HundredsSongsTiming"), "serd");
  SongList sl1 = SongList::from_string<msw_proto_song::Songs>(serd);
  SPDLOG_LOGGER_DEBUG(spdl::get("HundredsSongsTiming"), "deserialized ..");
}

#ifdef _WIN32
TEST(Cli, everythingBasic) {
  msw::helpers::CmdParse cmd_parse(
      L"swName --artist 1 --album \"spaces one\" --title ßtrangeTitleš --path C:\\one\\two");
  // msw::helpers::CmdParse cmd_parse(L"programName --artist aaa --album bbb --title ccc --path kkk");
  Song a_song = std::make_from_tuple<Song>(cmd_parse.song_data().to_tuple());
  ASSERT_EQ("1", a_song.artist());
  ASSERT_EQ("spaces one", a_song.album());
}

TEST(Cli, emptyAlbum) {
  msw::helpers::CmdParse cmd_parse(L"swName --artist 1 --album   \"\" --title ßtrangeTitleš --path C:\\one\\two");
  Song a_song = std::make_from_tuple<Song>(cmd_parse.song_data().to_tuple());
  ASSERT_EQ("", a_song.album());
}

#ifdef _MSC_VER
TEST(Misc, stackTraceOstreamDoesntCrash) {
  const msw::exceptions::AppStackWalker app_stack_walker;
  std::ostringstream one;
  one << app_stack_walker;
  SPDLOG_INFO(one.str());
  ASSERT_TRUE(one.str().length() > 0);
}

TEST(Misc, stackTraceOstreamDifferentSizes) {
  msw::exceptions::AppStackWalker app_stack_walker;

  std::vector<std::string> v(50);
  int generate_i = 0;
  std::generate(v.begin(), v.end(), [&generate_i]() { return "__gend .. " + std::to_string(generate_i++); });

  app_stack_walker.set_custom_value(std::move(v));
  SPDLOG_INFO("Attempt1: {}", app_stack_walker);

  app_stack_walker.set_custom_value({"one", "two", "three"});
  SPDLOG_INFO("Attempt2: {}", app_stack_walker);

  app_stack_walker.set_custom_value({"one"});
  SPDLOG_INFO("Attempt3: {}", app_stack_walker);

  app_stack_walker.set_custom_value({});
  SPDLOG_INFO("Attempt4 (empty): {}", app_stack_walker);

  app_stack_walker.set_custom_value({"final_one"});
  SPDLOG_INFO("Attempt4 .. final one: {}", app_stack_walker);
}
#endif

TEST(Misc, str_starts_with) {
  ASSERT_TRUE(msw::helpers::Utilities::starts_with("a11"s, "a", 1));
  ASSERT_FALSE(msw::helpers::Utilities::starts_with("a11"s, "b", 1));
  ASSERT_TRUE(msw::helpers::Utilities::starts_with("a11"s, "a1", 2));
  ASSERT_FALSE(msw::helpers::Utilities::starts_with("a11"s, "a2", 2));
}

TEST(tbd1, not_eq) {
  msw::StringProvider sp;
  Song song1(sp.get_str(), sp.get_str(), sp.get_str(), sp.get_str());
  Song song2(song1.album(), song1.artist(), song1.title(), "RandomFN");
  auto sm = song1.similarity(song2);

  SPDLOG_INFO("{}", msw::model::SongPartDifferences::SongSimilarityOstreamHelper{&sm, &song1, &song2});
  ASSERT_EQ(0, sm.path);
  ASSERT_EQ(3, sm.equal_fields);
}

TEST(SongModel, diff_or_same) {
  Song song1_1 = msw::StringProvider::song_with_playcnt("a", "b", "c", "d", 1);
  Song song1 = msw::StringProvider::song_with_playcnt("a", "b", "c", "d", 1);
  Song song2 = msw::StringProvider::song_with_playcnt("a", "b", "c", "d", 2);
  Song song3 = msw::StringProvider::song_with_playcnt("b", "b", "c", "d", 2);

  ASSERT_EQ(song1, song1_1);
  ASSERT_EQ(song1, song2);
}

TEST(SongModel, copy) {
  const Song song1 = msw::StringProvider::song_with_playcnt("a", "b", "c", "d", 1);

  msw::pg::song_list->write([&song1](SongList* mutated) { mutated->add_by_copying(song1.make_copy()); });

  auto inserted = msw::pg::song_list->read()->operator[](0);
  ASSERT_EQ(inserted, song1);
}

TEST(SongModel, manipulate_genre) {
  auto song1 = msw::StringProvider().get_simple_song();
  auto set_and_test = [&song1](const std::string& stringy, const char* const msg, int line) {
    song1.set_genre(std::string(stringy));
    ASSERT_EQ(stringy, song1.genre()) << "Msg: " << msg << " at " << line;
  };
  ASSERT_TRUE(song1.genre().empty()) << "Genre must be empty initially.";

  set_and_test("genre1", "first setting", __LINE__);
  set_and_test("genre2", "change the value", __LINE__);
  song1.add_tag("random1");
  song1.add_tag("random2");
  set_and_test("genre3", "added tags ... and changed genre", __LINE__);
  set_and_test("genre3", "set to equal ...", __LINE__);
  set_and_test("", "empty one", __LINE__);
  set_and_test("genre4", "not empty anymore.", __LINE__);
  song1.add_tag("random3");
  ASSERT_EQ("genre4", song1.genre()) << "Added tag. Genre must remain.";
  song1.remove_tag_strict("random2");
  ASSERT_EQ("genre4", song1.genre()) << "Removed some tag. Genre must remain.";
}

TEST(SongModel, manipulate_tags) {
  auto song1 = msw::StringProvider().get_simple_song();
  song1.add_tag("first");
  ASSERT_EQ("first", song1.delimited_tags());
  song1.add_tag("2");
  ASSERT_EQ("first,2", song1.delimited_tags());
  ASSERT_EQ("first;;;2", song1.delimited_tags(";;;"));

  song1.add_tag("three");
  ASSERT_EQ("first,2,three", song1.delimited_tags());

  song1.remove_tag_strict("2");
  ASSERT_EQ("first,three", song1.delimited_tags()) << "remove middle";

  song1.add_tag("2");
  ASSERT_EQ("first,three,2", song1.delimited_tags()) << "reordered";
  song1.add_tag("6");
  song1.add_tag("7");
  ASSERT_EQ("first,three,2,6,7", song1.delimited_tags()) << "two more";

  song1.remove_tag_strict("first");
  ASSERT_EQ("three,2,6,7", song1.delimited_tags()) << "remove at beginning (first)";

  song1.remove_tag_strict("7");
  ASSERT_EQ("three,2,6", song1.delimited_tags()) << "remove at end (last)";

  song1.remove_tag_strict("2");
  song1.remove_tag_strict("three");
  ASSERT_EQ("6", song1.delimited_tags()) << "One remains.";
  song1.remove_tag_strict("6");
  ASSERT_EQ("", song1.delimited_tags()) << "One remains.";
}

// class CliMissingSongItems : public testing::TestWithParam<const wchar_t*> {
//};
//
// TEST_P(CliMissingSongItems, emptyStringsCmd) {
//  msw::helpers::CmdParse missing_album(GetParam());
//  ASSERT_THROW(
//      std::make_from_tuple<Song>(missing_album.song_data().to_tuple()),
//      msw::exceptions::ApplicationError
//      );
//}
//
// INSTANTIATE_TEST_SUITE_P(CliMissingSongItemsEmptyStringCmd,
//                         CliMissingSongItems,
//                         testing::Values(
//                           L"swName --artist a1 --album --title ŠžTi --path C:\\q",
//                           L"swName --artist a1 --title ŠžTi --path C:\\q"
//                         ));
//

#endif
