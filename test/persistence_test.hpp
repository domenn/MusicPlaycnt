#pragma once

#include <gtest/gtest.h>

#include <src/data/accessor.hpp>
#include <src/data/pointers_to_globals.hpp>
#include <src/misc/spd_logging.hpp>
#include <test/util/data_for_tests.hpp>

TEST(Persistence, savesToString) {
  constexpr static char VALUE[] = "theUnitTestyArtist";
  msw::pg::handled_song->write([](msw::model::SongWithMetadata* mutated)
  {
    mutated->get_song().set_artist(VALUE);
  });
  
  ASSERT_EQ(msw::pg::handled_song->read()->get_song().artist(), VALUE);
  const std::string& serval = msw::pg::data_for_tests->serialized_;
  ASSERT_NE(serval.find(VALUE),
            std::string::npos) << "Test-content serialized string doesn't contain the expected word (" << VALUE << ")";
}
