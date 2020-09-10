#pragma once

#include <gtest/gtest.h>

#include <src/misc/spd_logging.hpp>
#include <src/win/windows_headers.hpp>

#include <src/misc/utilities.hpp>
#include <src/model/app_config.hpp>
#include <src/model/song.hpp>
#include <src/model/song_list.hpp>
#include <src/musicstuff/foo_np_log_parser.hpp>
#include <src/win/winapi_exceptions.hpp>
#include <test/util/string_provider.hpp>

using namespace std::string_literals;
using Song = msw::model::Song;
using SongList = msw::model::SongList;

class MockFooNpLogParser : public msw::musicstuffs::FooNpLogParser {
 public:
  const std::string line_to_parse_;

  MockFooNpLogParser(const msw::model::AppConfig& app_config, std::string line_to_parse)
      : FooNpLogParser(app_config), line_to_parse_(std::move(line_to_parse)) {}
};

class FoobarLineParse : public testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(FoobarLineParse, emptyStringsCmd) {
  const auto& param_pair = GetParam();
  msw::model::AppConfig tests_config;
  tests_config.set_my_defaults();
  MockFooNpLogParser parser(tests_config, param_pair.first);
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
        std::make_pair("Tue Sep 09 16:22:11 2020;playing: a ;; b ;; c ;;; d\nTue Sep 10 11:23:47 2020;stopped: e ;; f ;; g ;;; h"s, "b"s),
        std::make_pair("Tue Sep 09 16:22:11 2020;playing: a ;; b ;; c ;;; d\nTue Sep 10 11:23:47 2020;paused: e ;; f ;; g ;;; h"s, "f"s)
    ));

TEST(Fundamental, createSaveAndRestore) {
  msw_proto_song::Song proto_song;
  proto_song.set_album("oneAlbum");
  proto_song.set_artist("oneArtist");
  proto_song.set_title("TITTELLL!!!!");
  proto_song.set_path("OnePath ....");
  proto_song.set_playcnt(42);

  msw::model::Song song1(std::move(proto_song));
  std::string serd = song1.serialize();
  SPDLOG_INFO("Serialized first " + serd);

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
