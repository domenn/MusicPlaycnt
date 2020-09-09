#pragma once

#include <gtest/gtest.h>

#include <src/data/accessor.hpp>
#include <src/data/pointers_to_globals.hpp>
#include <src/misc/spd_logging.hpp>
#include <test/util/data_for_tests.hpp>
//#include "src/model/song_list.hpp"
//#include "src/model/song.hpp"
//#include "util/string_provider.hpp"
//#include "src/misc/utilities.hpp"
//#include "src/win/winapi_exceptions.hpp"
//#include "src/musicstuff/foo_np_log_parser.hpp"
//#include "src/model/app_config.hpp"

TEST(Persistence, savesToString) {
  constexpr static char VALUE[] = "theUnitTestyArtist";
  msw::pg::handled_song->write([](msw::model::SongWithMetadata* mutated)
  {
    mutated->get_song().set_artist(VALUE);
  });
  
  ASSERT_EQ(msw::pg::handled_song->read()->get_song().artist(), VALUE);
  const std::string& serval = reinterpret_cast<DataForTests*>(msw::pg::handled_song->data())->serialized_;
  ASSERT_NE(serval.find(VALUE),
            std::string::npos) << "Test-content serialized string doesn't contain the expected word (" << VALUE << ")";
}
